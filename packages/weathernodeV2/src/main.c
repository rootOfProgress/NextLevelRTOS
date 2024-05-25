#include "adc.h"
#include "gpio/gpio.h"
#include "test.h"
#include "spi.h"
#include "nrf24l01.h"
#include "rcc.h"
#include "uart.h"
#include "os_api.h"
#include "crc.h"
#include "exti.h"
#include "syscfg.h"
#include "am2302.h"
// #include "memory/memory_globals.h"

#define SV_YIELD_TASK __asm volatile ("mov r6, 2\n" \
                                  "svc 0\n")

#define READ_REGISTER(addr)     (*(volatile unsigned int *) (addr))
#define WRITE_REGISTER(addr, val) ((*(volatile unsigned int *) (addr)) = (unsigned int) (val))

typedef struct
{
  unsigned short version;
  unsigned short size;
} StructHeader_t;

typedef struct 
{
  char nodeNumber;
  char reserved[3];
} PayloadMetaData_t;

typedef struct 
{
  char totalLostPackages;
  char retransmitsForThisPackage;
  char signalStrength;
  char reserved;
  unsigned short batteryHealth;
  char currentChannel;
  char reserved1[3];
} DeviceEnvironmentData_t;

typedef struct NodeFrame 
{
  StructHeader_t header;
  PayloadMetaData_t meta;
  DeviceEnvironmentData_t environment;
  Am2302Readings_t readings;
} NodePackage_t;

Nrf24l01Registers_t nrf_registers;
Am2302Readings_t readings;
// NodePackage_t package;
GpioObject_t pinb;
GpioObject_t gpio_adc_a1;
GpioObject_t gpio_a3_notify;
TxConfig_t tx_config;
TxObserve_t observe;
unsigned short battery;

unsigned int tAckReceived;
char receivedAckPackage;
char outBuffer[32];

void  __attribute__((optimize("O0"))) tx_receive_isr()
{
  exti_acknowledge_interrupt(&pinb);
  receivedAckPackage = tx_ack_receive_isr(&nrf_registers);
}

void apply_nrf_config(Nrf24l01Registers_t *nrf_registers)
{

  /************ Disable AA ***************/
#ifdef SHOCKY
  nrf_registers->en_aa = 0x3F; // ENAA_P0
  nrf_registers->setup_retr = 7;
  nrf_registers->en_rxaddr = 0x3F;
#else
  nrf_registers->en_aa = 0;
  nrf_registers->en_rxaddr = 1;

  // payload size of ack: 1 byte
  nrf_registers->rx_pw_p0 = 1;

#endif
  /************ 1Mbps data rate, 0dBm ***************/
  nrf_registers->rf_setup = 6;

  /************ 5 byte address width ***************/
  nrf_registers->setup_aw = 3;

  /************ Channel 3 ***************/
  nrf_registers->rf_ch = 3;

  nrf_registers->config = 0; //(char) (1 << 2) | (char) (1 << 3);
  // LSB is written first, will result in bfcecccecc
  // char tx[5] = {0xCC, 0xCE, 0xCC, 0xCE, 0xBF};
  // char tx[5] = {0x9A, 0x78, 0x56, 0x34, 0x12};
  // char tx[5] = {0xc6, 0xc6, 0xc6, 0xc6, 0xc6};
  // nrf_registers->tx_addr = 0xC5C5C5C5;
  char tx[5] = {0xc5, 0xc5, 0xc5, 0xc5, 0xc5};
  for (unsigned int i = 0; i < sizeof(tx) / sizeof(char); i++)
    nrf_registers->tx_addr[i] = tx[i];

  char rx_p0[5] = {0xc8, 0xc8, 0xc8, 0xc8, 0xc8};
  // char rx_p0[5] = {0xc5, 0xc5, 0xc5, 0xc5, 0xc5};
  // char rx_p0[5] = {0xa1, 0xa1, 0xa1, 0xa1, 0xa1};
  for (unsigned int i = 0; i < sizeof(rx_p0) / sizeof(char); i++)
    nrf_registers->rx_addr_p0[i] = rx_p0[i];
  // for (int i = sizeof(rx_p0)/sizeof(char) - 1; i >= 0; i--)
}

static void init_irq()
{
  pinb.pin = 0;
  pinb.port = 'B';

  link_exti_src(tx_receive_isr, &pinb);
  syscfg_enable_clock();
  syscfg_exti_config_0_3(&pinb);
  exti_activate_ir_line(&pinb);
  exti_detect_falling_edge(&pinb);
}

char sendConfig()
{
  char outBuffer[32];
  for (unsigned int i = 0; i < 32; i++)
  {
    outBuffer[i] = 0;
  }
  RxConfig_t rxConfig;
  rxConfig.identifier = 0x12345678;
  rxConfig.configMask = OutputToUart;
  rxConfig.channel = 3;
  rxConfig.timeToSendAck = 0;
  rxConfig.timeToSettle = 0;
  char *payloadPtr = (char*) &rxConfig;
  for (unsigned int i = 0; i < sizeof(RxConfig_t); i++)
  {
    outBuffer[i] = payloadPtr[i];
  }
  return transmit_with_autoack(&tx_config, &receivedAckPackage, outBuffer);
}

void adc_isr()
{
    adc_acknowledge_interrupt();
    exti_acknowledge_interrupt(&gpio_adc_a1);
    battery = adc_read_regular_channel();
}

void led()
{
  while (1)
  {
    toggle_output_pin(&gpio_a3_notify);
    sleep(1000);
  }
}

int __attribute((section(".main"))) __attribute__((__noipa__)) __attribute__((optimize("O0"))) main(void)
{
  memset_byte((void*) outBuffer, 32, 0);
  NodePackage_t *package = (NodePackage_t*) outBuffer;

  package->header.size = sizeof(NodePackage_t) - sizeof(StructHeader_t);
  package->meta.nodeNumber = 0;

  append_os_core_function(read_timer);

  receivedAckPackage = 0;
  memset_byte((void*) &tx_config, sizeof(TxConfig_t), 0);

  tx_config.autoRetransmitDelay = 9000;
  tx_config.retransmitCount = 7;
  timeToSettle = 150;

  crc_activate();
  apply_nrf_config(&nrf_registers);
  configure_device(&nrf_registers, MASTER);
  init_irq();

  
  append_os_core_function(read_timer);
  am2302_init_peripherials(0, 'A');

  gpio_adc_a1.pin = 1;
  gpio_adc_a1.port = 'A';
  adc_init(&gpio_adc_a1);
  adc_enable_interrupts();
  adc_turn_on();
  link_adc_isr(adc_isr);

  gpio_a3_notify.pin = 3;
  gpio_a3_notify.port = 'A';
  init_gpio(&gpio_a3_notify);
  set_moder(&gpio_a3_notify, GeneralPurposeOutputMode);
  set_pin_on(&gpio_a3_notify);

  unsigned int am2302Retries;
  char *transmitterState;

  // @multiple pid sleep must be implemented/fixed (RTOS_MultipleSleep)
  // unsigned int pid = create_task(&led, 0);
  // join_task(pid);

  if (!sendConfig())
  {
    NodePackage_t *package = (NodePackage_t*) outBuffer;
    package->header.version = 99;
    print((char*) &outBuffer, sizeof(NodePackage_t));
    return 0;
  };

  while (1)
  {
    char *payloadPtr = outBuffer;
    am2302Retries = 0;

    // __asm ("CPSID I");
    while (am2302Retries++ < 100)
    {
      if (am2302_do_measurement(&package->readings))
      {
        break;
      }
    }

    observe = get_current_tx_state();

    package->environment.retransmitsForThisPackage = observe.retransmitCount;
    // @todo : don't cast datatypes
    package->environment.signalStrength = (char) observe.signalStrength;
    // @todo : don't cast datatypes
    package->environment.totalLostPackages = (char) observe.totalLostPackages;
    package->environment.batteryHealth = battery;
    package->environment.currentChannel = observe.currentChannel;

    // memcpy_byte((void*) payloadPtr, (void*) &package, sizeof(NodePackage_t));
    // payloadPtr += sizeof(NodePackage_t);

    // memcpy_byte((void*) payloadPtr, (void*) &readings, sizeof(Am2302Readings_t));
    // payloadPtr += sizeof(Am2302Readings_t);

    // memcpy_byte((void*) payloadPtr, (void*) &observe.totalLostPackages, sizeof(unsigned int));
    // payloadPtr += sizeof(unsigned int);

    // memcpy_byte((void*) payloadPtr, (void*) &observe.totalRetransmits, sizeof(unsigned int));
    // payloadPtr += sizeof(unsigned int);

    // memcpy_byte((void*) payloadPtr, (void*) &observe.signalStrength, sizeof(unsigned int));
    // payloadPtr += sizeof(unsigned int);

    // char nodeInfo = 0xAB;
    // memcpy_byte((void*) payloadPtr, (void*) &nodeInfo, sizeof(char));
    // payloadPtr += sizeof(char);

    transmit_with_autoack(&tx_config, &receivedAckPackage, outBuffer);
    print((char*) &outBuffer, sizeof(NodePackage_t));
    // request_channel_change(&tx_config, &receivedAckPackage);
    // __asm ("CPSIE I");
    adc_start_conv_regular();

    sleep(1000);
  }
  return 0;
}

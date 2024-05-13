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

Nrf24l01Registers_t nrf_registers;
Am2302Readings_t readings;
GpioObject_t pinb;
TxConfig_t tx_config;
TxObserve_t observe;

unsigned int tAckReceived;
char receivedAckPackage;
char outBuffer[32];

typedef struct 
{
  char payloadId;
  char packageNumber; //@not yet used
  char totalPackages; //@not yet used
  char reserved;
} PayloadMetaData_t;

typedef struct 
{
  char totalLostPackages;
  char maxRetransmits;
  char signalStrength;
  char batteryHealth;
} DeviceEnvironmentData_t;

typedef struct NodeFrame 
{
  PayloadMetaData_t metadata;
  DeviceEnvironmentData_t environmentdata;
  Am2302Measurements_t readings;
} NodeFrame_t;


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

int __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{
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

  unsigned int i, j, am2302Retries;
  char *transmitterState;

  if (!sendConfig())
  {
    observe.signalStrength = 9999;
    print((char*) &observe, sizeof(TxObserve_t)); 
    return 0;
  };

  while (1)
  {
    am2302Retries = 0;
    while (am2302Retries++ < 100)
    {
      if (am2302_do_measurement(&readings))
      {
        break;
      }
    }
    char *temperatureSensorData = (char*) &readings;
    
    j = 0;
    i = 0;

    char *payloadPtr = outBuffer;
    
    // for (i = 0; i < sizeof(Am2302Readings_t); i++) // 6byte
    // {
    //   outBuffer[j++] = temperatureSensorData[i];
    // }

    memcpy_byte((void*) payloadPtr, (void*) temperatureSensorData, sizeof(Am2302Readings_t));
    payloadPtr += sizeof(Am2302Readings_t);

    memcpy_byte((void*) payloadPtr, (void*) &observe.totalLostPackages, sizeof(unsigned int));
    payloadPtr += sizeof(unsigned int);

    // transmitterState = (char*) &observe.totalLostPackages;

    // for (i = 0; i < sizeof(unsigned int); i++) // 4byte
    // {
    //   outBuffer[j++] = transmitterState[i];
    // }

    memcpy_byte((void*) payloadPtr, (void*) &observe.totalRetransmits, sizeof(unsigned int));
    payloadPtr += sizeof(unsigned int);

    // transmitterState = (char*) &observe.totalRetransmits;

    // for (i = 0; i < sizeof(unsigned int); i++) // 4byte
    // {
    //   outBuffer[j++] = transmitterState[i];
    // }

    memcpy_byte((void*) payloadPtr, (void*) &observe.signalStrength, sizeof(unsigned int));
    payloadPtr += sizeof(unsigned int);
    // transmitterState = (char*) &observe.signalStrength;

    // for (i = 0; i < sizeof(unsigned int); i++) // 4byte
    // {
    //   outBuffer[j++] = transmitterState[i];
    // }


    char nodeInfo = 0xAB;
    memcpy_byte((void*) payloadPtr, (void*) &nodeInfo, sizeof(char));
    payloadPtr += sizeof(char);
    
    // transmitterState = (char*) &nodeInfo;
    // for (i = 0; i < sizeof(char); i++) // 1byte
    // {
    //   outBuffer[j++] = *transmitterState;
    // }

    transmit_with_autoack(&tx_config, &receivedAckPackage, outBuffer);
    observe = get_current_tx_state();
    // print((char*) &observe, sizeof(TxObserve_t));
    print((char*) &outBuffer, 19);
    sleep(1000);
  }
  
  NodeFrame_t myNodeFrame;
  memset_byte((void*) &myNodeFrame, sizeof(NodeFrame_t), 89);

  TxObserve_t observe = get_current_tx_state();
  print((char*) &observe, sizeof(TxObserve_t)); 


  return 0;
}

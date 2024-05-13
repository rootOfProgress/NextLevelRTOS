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

#define SV_YIELD_TASK __asm volatile ("mov r6, 2\n" \
                                  "svc 0\n")

#define READ_REGISTER(addr)     (*(volatile unsigned int *) (addr))
#define WRITE_REGISTER(addr, val) ((*(volatile unsigned int *) (addr)) = (unsigned int) (val))

Nrf24l01Registers_t nrf_registers;

GpioObject_t pinb;
TxObserve_t observe;
TxConfig_t tx_config;
unsigned int tAckReceived;
char receivedAckPackage;

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

void normal(char *outBuffer, GpioObject_t *gpio_b7)
{
  unsigned int total = 0;
  for (int i = 0; i < 1; i++)
  {
    unsigned int payload = read_timer();
    // unsigned int payload = 0x87654321;
    // unsigned int payload = 0x12345678;
    char *payloadPtr = (char*) &payload;
    for (unsigned int i = 0; i < sizeof(unsigned int); i++)
    {
      outBuffer[i] = payloadPtr[i];
    }
    unsigned int tStart = read_timer();
    // set_pin_on(gpio_b7);
    transmit_with_autoack(&tx_config, &receivedAckPackage, outBuffer);
    // set_pin_off(gpio_b7);
    unsigned int tEnd = read_timer();
    total += tEnd - tStart;
  }
  observe = get_current_tx_state();
  observe.totalElapsed = (total) >> 10;
  print((char*) &observe, sizeof(TxObserve_t));
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
  rxConfig.configMask = ChangeChannel | OutputToUart;
  rxConfig.channel = 4;
  rxConfig.timeToSendAck = 250;
  rxConfig.timeToSettle = 50;
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
  memset_byte((void*) &observe, sizeof(TxObserve_t), 0);

  tx_config.autoRetransmitDelay = 9000;
  tx_config.retransmitCount = 7;
  timeToSettle = 150;

  crc_activate();
  apply_nrf_config(&nrf_registers);
  configure_device(&nrf_registers, MASTER);
  init_irq();
  GpioObject_t gpio_b1;
  GpioObject_t gpio_b7;

  gpio_b1.pin = 2;
  gpio_b1.port = 'B';
  init_gpio(&gpio_b1);
  set_moder(&gpio_b1, InputMode);
  set_otyper(&gpio_b1, PushPull);
  set_pupdr(&gpio_b1, Nothing);
  set_speed(&gpio_b1, High);

  gpio_b7.pin = 7;
  gpio_b7.port = 'B';
  init_gpio(&gpio_b7);
  set_moder(&gpio_b7, GeneralPurposeOutputMode);
  set_otyper(&gpio_b7, PushPull);
  set_pupdr(&gpio_b7, Nothing);
  set_speed(&gpio_b7, High);

  char outBuffer[32];
  for (unsigned int i = 0; i < 32; i++)
  {
    outBuffer[i] = 0;
  }

  unsigned int btnPressed = 0;
  unsigned int lastPressed = 0;

  // if (sendConfig())
  // {
  //   if (!change_channel(4))
  //   {
  //     observe.totalLostPackages = 99999;
  //     print((char*) &observe, sizeof(TxObserve_t));
  //     return 0;
  //   };
  // }
  while (1)
  {
    if (!read_pin(&gpio_b1))
    {
      normal(outBuffer, &gpio_b7);
      sleep(20);
      btnPressed = 0;
    }
    SV_YIELD_TASK;
  }
  return 0;
}

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
TxObserve_t tx_observe;
TxConfig_t tx_config;
unsigned int tAckReceived;
char receivedAckPackage;

void  __attribute__((optimize("O0"))) tx_receive_isr()
{
  // asm("bkpt");
  exti_acknowledge_interrupt(&pinb);
  tx_observe.timeUntilAckArrived = read_timer();
  receivedAckPackage = tx_ack_receive_isr(&nrf_registers);
  // char status = get_nrf_status();
  // if (status & (1 << 5))
  // {
  //   clear_tx_ds_flag();
  //   return;
  // }
  // unsigned int fifo_status = 0;
  // char rx_answer[32];
  // while (!((fifo_status = get_nrf_fifo()) & 1))
  // {
  //   if (check_for_received_data(&nrf_registers, rx_answer))
  //   {
  //     receivedAckPackage = 1;
  //   }
  // }
  //   // clear_tx_ds_flag();

  // clear_rx_dr_flag();
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

int __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{
  // now done in driver!
  // init_spi();
  receivedAckPackage = 0;
  memset_byte((void*) &tx_observe, sizeof(TxObserve_t), 0);
  memset_byte((void*) &tx_config, sizeof(TxConfig_t), 0);

  tx_config.autoRetransmitDelay = 16000; // 12 ms
  tx_config.retransmitCount = 7;

  crc_activate();

  apply_nrf_config(&nrf_registers);
  // config.

  configure_device(&nrf_registers, MASTER);
  init_irq();

  char outBuffer[32];
  crc_reset();
  char* p = "WasIstLoGREAT!";
  for (unsigned int i = 0; i < 32; i++)
  {
    outBuffer[i] = 0;
  }

  for (unsigned int i = 0; i < 14; i++)
  {
    outBuffer[i] = p[i];
    // crc_feed((unsigned int)p[i]);
  }

  for (unsigned int i = 0; i < 27; i++)
  {
    crc_feed((unsigned int)outBuffer[i]);
  }

  unsigned int crc = crc_read();
  char *crc_ptr = (char*) &crc;

  for (unsigned int i = 0; i < sizeof(unsigned int); i++)
  {
    outBuffer[27 + i] = crc_ptr[sizeof(unsigned int) - 1 - i];
    // outBuffer[27 + i] = crc_ptr[i];
  }

  // char* t = "llkj;vmdd";
  // char* r = "ipoijvmdd";

  // load_tx_buffer(32, outBuffer);

  // load_tx_buffer(6, t);
  // load_tx_buffer(6, r);
  // transmit_single_package();
  // transmit_all_packages();

  // unsigned int autoRetransmitDelay = 4000;
  // unsigned int rounds = 0;

  tx_observe.retransmitCount = 0;
  receivedAckPackage = 0;
  transmit_with_autoack(&tx_observe, &tx_config, &receivedAckPackage, outBuffer, read_timer);
  // while (tx_observe.retransmitCount < tx_config.retransmitCount)
  // {
  //   load_tx_buffer(32, outBuffer);
  //   transmit_single_package();
  //   enable_rx_and_listen();
  //   unsigned int tStart = read_timer();
  //   unsigned int tEnd = 0;
  //   while (((tEnd = read_timer()) - tStart) < tx_config.autoRetransmitDelay) {

  //   }
    
  //   disable_rx();
  //   if (!receivedAckPackage)
  //   {
  //     tx_observe.retransmitCount++;
  //   }
  //   else
  //   {
  //     tx_observe.timeUntilAckArrived = tAckReceived - tStart;
  //     break;
  //   }
  // }

  // if (tx_observe.retransmitCount == tx_config.retransmitCount)
  // {
  //   tx_observe.totalLostPackages++;
  // }
  // if (tx_observe.retransmitCount > tx_observe.maxRetransmits)
  // {
  //   tx_observe.maxRetransmits = tx_observe.retransmitCount;
  // }
  // tx_observe.retransmitCount = 0;
  print((char*) &tx_observe, sizeof(TxObserve_t));
  // asm("bkpt");

  return 0;
}

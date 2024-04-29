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
#define SV_YIELD_TASK __asm volatile ("mov r6, 2\n" \
                                  "svc 0\n")

#define READ_REGISTER(addr)     (*(volatile unsigned int *) (addr))
#define WRITE_REGISTER(addr, val) ((*(volatile unsigned int *) (addr)) = (unsigned int) (val))

Nrf24l01Registers_t nrf_registers;

GpioObject_t pinb;
TxConfig_t tx_config;
unsigned int tAckReceived;
char receivedAckPackage;

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

void send(char *outBuffer)
{
  // receivedAckPackage = 0;
  nrf_flush_tx();
  transmit_with_autoack(&tx_config, &receivedAckPackage, outBuffer);
}

int __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{
  append_os_core_function(read_timer);

  receivedAckPackage = 0;
  memset_byte((void*) &tx_config, sizeof(TxConfig_t), 0);

  tx_config.autoRetransmitDelay = 16000;
  tx_config.retransmitCount = 7;

  crc_activate();
  apply_nrf_config(&nrf_registers);
  configure_device(&nrf_registers, MASTER);
  init_irq();

  char outBuffer[32];
  
  // for (unsigned int i = 0; i < 32; i++)
  // {
  //   outBuffer[i] = 0;
  // }

  unsigned int total = 0;
  
  struct pay 
  {
    unsigned int fuckit[8];
  };

  struct pay well;
  well.fuckit[0] = 0xEE22EE22;
  well.fuckit[1] = 0xFF11FF11;
  well.fuckit[2] = 0x22334455;
  well.fuckit[3] = 0x33445566;
  well.fuckit[4] = 0x44556677;
  well.fuckit[5] = 0x55667788;
  well.fuckit[6] = 0x66778899;
  well.fuckit[7] = 0x778899AA;

char array[40];
for (int i = 0; i < 32; i++) {
    array[i] = (i % 32) + 1;
}



  // unsigned int payload = 0x99887744;
  // asm("bkpt");
  load_tx_buffer(17, array);
  transmit_single_package();
  NodeFrame_t myNodeFrame;
  memset_byte((void*) &myNodeFrame, sizeof(NodeFrame_t), 89);
  // Initializing and assigning increasing numbers
  // myNodeFrame.metadata.payloadId = 1;
  // myNodeFrame.metadata.packageNumber = 43;
  // myNodeFrame.metadata.totalPackages = 3;
  // myNodeFrame.metadata.reserved = 0;
  

  // myNodeFrame.environmentdata.totalLostPackages = 4;
  // myNodeFrame.environmentdata.maxRetransmits = 5;
  // myNodeFrame.environmentdata.signalStrength = 6;
  // myNodeFrame.environmentdata.batteryHealth = 7;

  // myNodeFrame.readings.humidity = 8;
  // myNodeFrame.readings.temperature = 9;

  // char *payloadPtr = (char*) &myNodeFrame;
  // for (unsigned int i = 0; i < sizeof(NodeFrame_t); i++)
  // {
  //   outBuffer[i] = payloadPtr[i];
  // }
  // unsigned int tStart = read_timer();
  // send(outBuffer);
  // unsigned int tEnd = read_timer();
  // total += tEnd - tStart;

  TxObserve_t observe = get_current_tx_state();
  observe.totalElapsed = (total) >> 10;
  print((char*) &observe, sizeof(TxObserve_t)); 


  return 0;
}

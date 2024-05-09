#include "gpio/gpio.h"
#include "spi.h"
#include "nrf24l01.h"
#include "rcc.h"
#include "uart.h"
// #include "os_api.h"
#include "exti.h"
#include "syscfg.h"
#include "gpio_isr.h"
#include "crc.h"
#include "scheduler_public.h"

#define SV_YIELD_TASK __asm volatile ("mov r6, 2\n" \
                                  "svc 0\n")

#define READ_REGISTER(addr)     (*(volatile unsigned int *) (addr))
#define WRITE_REGISTER(addr, val) ((*(volatile unsigned int *) (addr)) = (unsigned int) (val))

Nrf24l01Registers_t nrf24l01_regs;
Nrf24l01Registers_t nrf_startup_config;
static unsigned int lastReceivedCRC;
static unsigned int ackPackagePreloaded;
GpioObject_t pinb;
char ack = 'a';
char rx_answer[33];
GpioObject_t gpio_b8;
char *receiveBuffer;
// char rx_answer[16];

void apply_nrf_config(Nrf24l01Registers_t *nrf_registers)
{

  /************ Disable AA ***************/
  nrf_registers->en_aa = 0;

  /************ 1Mbps data rate, 0dBm ***************/
  nrf_registers->rf_setup = 6;

  /************ 5 byte address width ***************/
  nrf_registers->setup_aw = 3;

  /************ Channel 2 ***************/
  nrf_registers->rf_ch = 3;

  nrf_registers->en_rxaddr = 3;
  nrf_registers->rx_pw_p0 = 31;
  nrf_registers->rx_pw_p1 = 6;
  nrf_registers->config = 0;

  // LSB is written first, will result in bfcecccecc
  // char tx[5] = {0xCC, 0xCE, 0xCC, 0xCE, 0xBF};
  // char tx[5] = {0xa1, 0xa1, 0xa1, 0xa1, 0xa1};
  // nrf_registers->rx_addr_p0 = 0xC5C5C5C5;
  char tx[5] = {0xc8, 0xc8, 0xc8, 0xc8, 0xc8};
  for (unsigned int i = 0; i < sizeof(tx) / sizeof(char); i++)
    nrf_registers->tx_addr[i] = tx[i];
  // for (int i = sizeof(tx)/sizeof(char) - 1; i >= 0 ; i--)

  char rx_p0[5] = {0xc5, 0xc5, 0xc5, 0xc5, 0xc5};
  for (unsigned int i = 0; i < 5 /* sizeof(rx_p1)/sizeof(char) */; i++)
    nrf_registers->rx_addr_p0[i] = rx_p0[i];

  char rx_p1[5] = {0xc6, 0xc6, 0xc6, 0xc6, 0xc6};
  for (unsigned int i = 0; i < 5 /* sizeof(rx_p1)/sizeof(char) */; i++)
    nrf_registers->rx_addr_p1[i] = rx_p1[i];
  // for (int i = sizeof(rx_p0)/sizeof(char) - 1; i >= 0; i--)
}

typedef struct rxinfo
{
  unsigned int pipe;
  char content[32];
} rxinfo_t;

rxinfo_t rx_data[32];
static unsigned int index = 0;
static unsigned int uartOutputIsEnabled = 0;

void memcpy_custom(void *dest, const void *src, unsigned int n)
{
  char *cdest = (char *)dest;
  const char *csrc = (const char *)src;

  // Copy each byte from source to destination
  for (unsigned int i = 0; i < n; ++i)
  {
    cdest[i] = csrc[i];
  }
}

// Function to reverse the byte order of a 32-bit unsigned integer
unsigned int reverse_byte_order(unsigned int num)
{
  return ((num >> 24) & 0x000000FF) |
         ((num >> 8) & 0x0000FF00) |
         ((num << 8) & 0x00FF0000) |
         ((num << 24) & 0xFF000000);
}

unsigned int timeToSendAck;

void rx_receive_isr()
{
  exti_acknowledge_interrupt(&pinb);
  for (int i = 0; i < 33; i++)
  {
    rx_answer[i] = 0;
  }

  while (!(get_nrf_fifo() & 1))
  {
    if (check_for_received_data(&nrf_startup_config, rx_answer))
    {

      // @todo: better feed 4byte wise
      crc_reset();
      for (unsigned int i = 0; i < 27; i++)
      {
        crc_feed((unsigned int)rx_answer[i]);
      }

      unsigned int crc_expected = crc_read();
      unsigned int crc = 0;
      memcpy_custom(&crc, &rx_answer[27], sizeof(unsigned int)); // Ensure proper endianness and alignment

      crc = reverse_byte_order(crc);
      // @todo: discard package if ack gots lost
      // @todo: if crc==0, then no ack is required
      // if (crc == crc_expected)
      if (crc == crc_expected)
      {
        disable_rx();

        if (!ackPackagePreloaded)
        {
          load_tx_buffer(1, &ack);
          ackPackagePreloaded = 1;
        }
        set_pin_on(&gpio_b8);
        transmit_all_packages();
        set_pin_off(&gpio_b8);
        crc_reset();
        ackPackagePreloaded = 0;

        RxConfig_t *rxConfig = (RxConfig_t*) rx_answer;
        if (rxConfig->identifier == 0x12345678)
        {
          uartOutputIsEnabled  = rxConfig->printUart;
          timeToSettle = rxConfig->timeToSettle;
          timeToSendAck = rxConfig->timeToSendAck;
        }
        if (uartOutputIsEnabled)
        {
          print(rx_answer, 27);
        }

        lastReceivedCRC = crc;
        enable_rx_and_listen();
      }

    }
    else
    {
      break;
    }
  }
  clear_rx_dr_flag();
}

int __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) nrf2401_receive(void)
{
  // driver handels that
  // init_spi();
  index = 0;
  lastReceivedCRC = 0;
  ackPackagePreloaded = 0;
  uartOutputIsEnabled = 0;
  timeToSendAck = 0;
  append_os_core_function(read_global_timer);
  // Nrf24l01Registers_t nrf_startup_config;
  // Nrf24l01Registers_t nrf_current_config;

  // memset_byte((void*) &nrf_current_config, sizeof(Nrf24l01Registers_t), 0x0);
  // memset_byte((void*) &nrf_startup_config, sizeof(Nrf24l01Registers_t), 0x0);
  Nrf24l01Registers_t current_nrf_config;
  char* ptr = (char*) &nrf_startup_config;
  char* ptr1 = (char*) &current_nrf_config;
  for (unsigned int i = 0; i < sizeof(Nrf24l01Registers_t); i++)
  {
    ptr[i] = 0;
    ptr1[i] = 0;
    /* code */
  }
  apply_nrf_config(&nrf_startup_config);

  if (!configure_device(&nrf_startup_config, SLAVE))
  {
    // todo: turn on some light
    asm("bkpt");
  }

  for (int i = 0; i < 1000; i++)
  {
    /* code */
  }

  // task_sleep(10);
  crc_activate();
  // receiveBuffer = (char*) allocate(32768);

  nrf_flush_rx();
  clear_rx_dr_flag();

  start_listening();
  gpio_b8.pin = 8;
  gpio_b8.port = 'B';
  init_gpio(&gpio_b8);
  set_moder(&gpio_b8, GeneralPurposeOutputMode);
  set_otyper(&gpio_b8, PushPull);
  set_pupdr(&gpio_b8, Nothing);
  set_speed(&gpio_b8, High);
#define IRQ
#ifdef IRQ
  pinb.pin = 0;
  pinb.port = 'B';

  link_exti_src(rx_receive_isr, &pinb);
  syscfg_enable_clock();
  syscfg_exti_config_0_3(&pinb);
  exti_activate_ir_line(&pinb);
  exti_detect_falling_edge(&pinb);
  while (1)
  {
    if (!ackPackagePreloaded)
    {
      load_tx_buffer(1, &ack);
      ackPackagePreloaded = 1;
    }
    crc_reset();
    // get_nrf_config(&current_nrf_config);
    SV_YIELD_TASK;
  }

#else
  sleep(10);
  while (1)
  {
    sleep(1);

    while (!(get_nrf_fifo() & 1))
    {
      if (check_for_received_data(&nrf_startup_config, rx_answer))
      {
        asm("bkpt");
        clear_rx_dr_flag();
      }
    }
    sleep(1);

    SV_YIELD_TASK;
  }
#endif

  return 0;
}

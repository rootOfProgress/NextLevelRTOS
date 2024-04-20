#include "nrf24l01.h"
#include "nrf24l01_privates.h"
#include "gpio/gpio.h"
#include "spi.h"
#include "crc.h"

GpioObject_t gpio_pa5_ce;
TxObserve_t tx_observe;
unsigned int (*osCoreFunctions[4])() = {  };

void nrf_power_off()
{
  clear_bit_nrf_register(CONFIG, 1);
}

void nrf_power_on()
{
  set_bit_nrf_register(CONFIG, 1);
}

void append_os_core_function(unsigned int (*function)())
{
  static unsigned int current_ptr_index = 0;
  // asm("bkpt");
  // @todo
  osCoreFunctions[current_ptr_index++] = function;
}

char get_nrf_status(void)
{
  return get_nrf_register(STATUS);
}
char get_nrf_fifo(void)
{
  return get_nrf_register(FIFO_STATUS);
}
char get_nrf_rpd(void)
{
  return get_nrf_register(RPD);
}

TxObserve_t get_current_tx_state(void)
{
  return tx_observe;
}

void get_nrf_config(Nrf24l01Registers_t* current_nrf_config)
{
  current_nrf_config->config = get_nrf_register(CONFIG);
  current_nrf_config->en_aa = get_nrf_register(EN_AA);
  current_nrf_config->en_rxaddr = get_nrf_register(EN_RXADDR);
  current_nrf_config->setup_aw = get_nrf_register(SETUP_AW);
  current_nrf_config->setup_retr = get_nrf_register(SETUP_RETR);
  current_nrf_config->rf_ch = get_nrf_register(RF_CH);
  current_nrf_config->rf_setup = get_nrf_register(RF_SETUP);
  current_nrf_config->status = get_nrf_register(STATUS);
  current_nrf_config->observe_tx = get_nrf_register(OBSERVE_TX);
  current_nrf_config->rpd = get_nrf_register(RPD);
  get_nrf_register_long(RX_ADDR_P0, current_nrf_config->rx_addr_p0);
  get_nrf_register_long(RX_ADDR_P1, current_nrf_config->rx_addr_p1);
  current_nrf_config->rx_addr_p2 = get_nrf_register(RX_ADDR_P2);
  current_nrf_config->rx_addr_p3 = get_nrf_register(RX_ADDR_P3);
  current_nrf_config->rx_addr_p4 = get_nrf_register(RX_ADDR_P4);
  current_nrf_config->rx_addr_p5 = get_nrf_register(RX_ADDR_P5);
  get_nrf_register_long(TX_ADDR, current_nrf_config->tx_addr);
  current_nrf_config->rx_pw_p0 = get_nrf_register(RX_PW_P0);
  current_nrf_config->rx_pw_p1 = get_nrf_register(RX_PW_P1);
  current_nrf_config->rx_pw_p2 = get_nrf_register(RX_PW_P2);
  current_nrf_config->rx_pw_p3 = get_nrf_register(RX_PW_P3);
  current_nrf_config->rx_pw_p4 = get_nrf_register(RX_PW_P4);
  current_nrf_config->rx_pw_p5 = get_nrf_register(RX_PW_P5);
  current_nrf_config->fifo_status = get_nrf_register(FIFO_STATUS);
  current_nrf_config->dyndp = get_nrf_register(DYNDP);
  current_nrf_config->feature = get_nrf_register(FEATURE);

}

void clear_ir_maxrt_flag(void)
{
  set_bit_nrf_register(STATUS, 4);
}

unsigned int enable_crc(void)
{
  set_bit_nrf_register(CONFIG, 3);
  return (get_nrf_register(CONFIG) & (1 << 3)) != 0;
}

unsigned int disable_crc(void)
{
  clear_bit_nrf_register(CONFIG, 3);
  return (get_nrf_register(CONFIG) & (1 << 3)) == 0;
}

char configure_device(Nrf24l01Registers_t* nrf_regs, __attribute__((unused)) OperatingMode_t mode)
{
  init_spi();
  // @todo
  tx_observe.bytesSend = 0;
  gpio_pa5_ce.port = 'A';
  gpio_pa5_ce.pin = 5;
  init_gpio(&gpio_pa5_ce);
  unset_ce();
  nrf_power_off();
  replace_nrf_register(CONFIG, nrf_regs->config);
  if (mode == SLAVE)
  {
    set_bit_nrf_register(CONFIG, PRIM_RX);
  }
  else
  {
    clear_bit_nrf_register(CONFIG, PRIM_RX);
  }
  replace_nrf_register(EN_RXADDR, nrf_regs->en_rxaddr);

  // if (mode == SLAVE || nrf_regs->en_aa)
  // {
  //   replace_nrf_register(EN_RXADDR, nrf_regs->en_rxaddr);
  // }

  if (!disable_crc())
  {
    return 0;
  }
// clear_bit_nrf_register(CONFIG, 3);
  set_nrf_register_long(RX_ADDR_P0, nrf_regs->rx_addr_p0);
  set_nrf_register_long(RX_ADDR_P1, nrf_regs->rx_addr_p1);
  // replace_nrf_register(RX_ADDR_P2, nrf_regs->rx_addr_p2);
  // replace_nrf_register(RX_ADDR_P3, nrf_regs->rx_addr_p3);
  // replace_nrf_register(RX_ADDR_P4, nrf_regs->rx_addr_p4);
  // replace_nrf_register(RX_ADDR_P5, nrf_regs->rx_addr_p5);
  replace_nrf_register(RX_PW_P0, nrf_regs->rx_pw_p0);
  replace_nrf_register(RX_PW_P1, nrf_regs->rx_pw_p1);
  set_nrf_register_long(TX_ADDR, nrf_regs->tx_addr);

  replace_nrf_register(RF_CH, nrf_regs->rf_ch);
  replace_nrf_register(RF_SETUP, nrf_regs->rf_setup);
  replace_nrf_register(EN_AA, nrf_regs->en_aa);
  replace_nrf_register(EN_RXADDR, nrf_regs->en_rxaddr);
  replace_nrf_register(SETUP_AW, nrf_regs->setup_aw);
  replace_nrf_register(SETUP_RETR, nrf_regs->setup_retr);
  // replace_nrf_register(CONFIG, nrf_regs->config);
  
  nrf_power_on();

  {
    Nrf24l01Registers_t config;
    get_nrf_config(&config);

    if (config.rf_ch != nrf_regs->rf_ch ||
        config.rf_setup != nrf_regs->rf_setup ||
        config.setup_aw != nrf_regs->setup_aw ||
        config.en_rxaddr != nrf_regs->en_rxaddr
       )
    {
      return 0;
    }
  }

  return 1;
}

void unset_ce()
{
  set_pin_off(&gpio_pa5_ce);
}

void set_ce()
{
  set_pin_on(&gpio_pa5_ce);
}

void start_listening()
{
  set_ce();
}

void stop_listening()
{
  unset_ce();
}

void nrf_flush_rx(void)
{
  transfer_write(-1, 0, FlushRX, (void*) 0);
}

static unsigned int check_tx_availability(void)
{
  char fifo_status = get_nrf_register(FIFO_STATUS);

  if (fifo_status & (1 << 4)) // fifo empty
  {
    return 0;
  }

  char nrf_status = get_nrf_register(STATUS);
  if (nrf_status & (1 << 4)) // max_rt
  {
    clear_ir_maxrt_flag();
  }
  return 1;
}

unsigned int load_tx_buffer(unsigned int length, char* payload)
{
  char fifo_status = get_nrf_register(FIFO_STATUS);
  if (fifo_status & (1 << 5)) // fifo full
  {
    return 0;
  }
  transfer_write(-1, length, WTxPayload, payload);
  return 1;
}

unsigned int transmit_single_package(void)
{
  if (!check_tx_availability())
  {
    return 0;
  }
  set_ce();

  for (int i = 0; i < 10; i++)
  {
    // @todo replace with sleep as soon as fw is updated
  }

  unset_ce();
  return 1;
}

unsigned int transmit_all_packages(void)
{
  if (!check_tx_availability())
  {
    return 0;
  }
  set_ce();
  while (!(get_nrf_register(FIFO_STATUS) & (1 << 4))){}
  unset_ce();
  return 1;
}

void transmit_with_autoack(TxConfig_t *tx_config, 
                           char *receivedAckPackage,
                           char *outBuffer)
{
  crc_reset();
  for (unsigned int i = 0; i < 27; i++)
  {
    crc_feed((unsigned int)outBuffer[i]);
  }
  unsigned int crc = crc_read();
  char *crc_ptr = (char*) &crc;

  for (unsigned int i = 0; i < sizeof(unsigned int); i++)
  {
    outBuffer[27 + i] = crc_ptr[sizeof(unsigned int) - 1 - i];
  }

  while (tx_observe.retransmitCount < tx_config->retransmitCount)
  {
    load_tx_buffer(32, outBuffer);
    transmit_single_package();
    enable_rx_and_listen();
    unsigned int tStart = osCoreFunctions[readTimerFunctionPtr]();
    unsigned int tEnd = 0;
    while (((tEnd = osCoreFunctions[readTimerFunctionPtr]()) - tStart) < tx_config->autoRetransmitDelay 
            && !(*receivedAckPackage)) {}
    
    disable_rx();
    if (!(*receivedAckPackage))
    {
      tx_observe.retransmitCount++;
    }
    else
    {
      tx_observe.timeUntilAckArrived -= tStart;
      // tx_observe.timeUntilAckArrived >>= 10;
      break;
    }
  }

  if (tx_observe.retransmitCount == tx_config->retransmitCount)
  {
    tx_observe.totalLostPackages++;
  }
  if (tx_observe.retransmitCount > tx_observe.maxRetransmits)
  {
    tx_observe.maxRetransmits = tx_observe.retransmitCount;
  }
  *receivedAckPackage = 0;
  tx_observe.retransmitCount = 0;
  tx_observe.bytesSend += 28;
}

unsigned int __attribute__((optimize("O0"))) tx_ack_receive_isr(Nrf24l01Registers_t *nrf_registers)
{
  tx_observe.timeUntilAckArrived = osCoreFunctions[readTimerFunctionPtr]();
  clear_rx_dr_flag();
  char status = get_nrf_status();
  if (status & (1 << 5))
  {
    clear_tx_ds_flag();
    return 0;
  }
  unsigned int fifo_status = 0;
  char rx_answer[32];
  while (!((fifo_status = get_nrf_fifo()) & 1))
  {
    if (check_for_received_data(nrf_registers, rx_answer))
    {
      return 1;
    }
  }
  return 0;
}

void enable_rx_and_listen()
{
  nrf_power_off();
  set_bit_nrf_register(CONFIG, PRIM_RX);
  nrf_power_on();
  start_listening();
}

void disable_rx()
{
  nrf_power_off();
  clear_bit_nrf_register(CONFIG, PRIM_RX);
  nrf_power_on();
  stop_listening();
}

void clear_rx_dr_flag(void)
{
  set_bit_nrf_register(STATUS, 6);
}

void clear_tx_ds_flag(void)
{
  set_bit_nrf_register(STATUS, 5);
}

void nrf_receive_payload(unsigned int payload_length, char* buffer)
{
  transfer_read_wbuffer(-1, payload_length, RRxPayload, buffer);
}

char check_for_received_data(Nrf24l01Registers_t* config, char* response_buffer)
{
  char ret = 1;
  int pipe = 7;
  pipe = (get_nrf_status() >> 1) & 0x7;
  if (pipe >= 0 && pipe <= 5)
  {
    stop_listening();

    switch (pipe)
    {
    case 0:
      nrf_receive_payload(config->rx_pw_p0, response_buffer);
      break;
    case 1:
      nrf_receive_payload(config->rx_pw_p1, response_buffer);
      break;
    case 2:
      nrf_receive_payload(config->rx_pw_p2, response_buffer);
      break;
    case 3:
      nrf_receive_payload(config->rx_pw_p3, response_buffer);
      break;
    case 4:
      nrf_receive_payload(config->rx_pw_p4, response_buffer);
      break;
    case 5:
      nrf_receive_payload(config->rx_pw_p5, response_buffer);
      break;
    default:
      break;
    }
    start_listening();
  }
  else
  {
    ret = 0;
  }
  return ret;
}

#include "nrf24l01.h"
#include "nrf_driver.h"

Nrf24l01Registers_t current_nrf_config;


void nrf_power_off()
{
    clear_bit_nrf_register(CONFIG, 1);
}

void nrf_power_on()
{
    transfer(CONFIG, (char[1]) {0x2}, 1, write_register);

    // settle >1,5 msec
    // sleep(2);

    transfer(CONFIG, (char[1]) {0x2}, 1, read_register);
    nrf24l01_regs.config = receive_buffer[0];
}

void get_nrf_config(void)
{
    current_nrf_config.config = get_nrf_register(CONFIG);
    current_nrf_config.en_aa = get_nrf_register(EN_AA);
    current_nrf_config.en_rxaddr = get_nrf_register(EN_RXADDR);
    current_nrf_config.setup_aw = get_nrf_register(SETUP_AW);
    current_nrf_config.setup_retr = get_nrf_register(SETUP_RETR);
    current_nrf_config.rf_ch = get_nrf_register(RF_CH);
    current_nrf_config.rf_setup = get_nrf_register(RF_SETUP);
    current_nrf_config.status = get_nrf_register(STATUS);
    current_nrf_config.observe_tx = get_nrf_register(OBSERVE_TX);
    current_nrf_config.rpd = get_nrf_register(RPD);
    get_nrf_register_long(RX_ADDR_P0, current_nrf_config.rx_addr_p0);
    get_nrf_register_long(RX_ADDR_P1, current_nrf_config.rx_addr_p1);
    // current_nrf_config.rx_addr_p1 = get_nrf_register(RX_ADDR_P1);
    current_nrf_config.rx_addr_p2 = get_nrf_register(RX_ADDR_P2);
    current_nrf_config.rx_addr_p3 = get_nrf_register(RX_ADDR_P3);
    current_nrf_config.rx_addr_p4 = get_nrf_register(RX_ADDR_P4);
    current_nrf_config.rx_addr_p5 = get_nrf_register(RX_ADDR_P5);
    get_nrf_register_long(TX_ADDR, current_nrf_config.tx_addr);
    current_nrf_config.rx_pw_p0 = get_nrf_register(RX_PW_P0);
    current_nrf_config.rx_pw_p1 = get_nrf_register(RX_PW_P1);
    current_nrf_config.rx_pw_p2 = get_nrf_register(RX_PW_P2);
    current_nrf_config.rx_pw_p3 = get_nrf_register(RX_PW_P3);
    current_nrf_config.rx_pw_p4 = get_nrf_register(RX_PW_P4);
    current_nrf_config.rx_pw_p5 = get_nrf_register(RX_PW_P5);
    current_nrf_config.fifo_status = get_nrf_register(FIFO_STATUS);
    current_nrf_config.dyndp = get_nrf_register(DYNDP);
    current_nrf_config.feature = get_nrf_register(FEATURE);

}

char configure_device(Nrf24l01Registers_t* nrf_regs, __attribute__((unused)) OperatingMode_t mode)
{
    replace_nrf_register(RF_SETUP, nrf_regs->rf_setup);
    replace_nrf_register(EN_AA, nrf_regs->en_aa);
    set_nrf_register_long(TX_ADDR, nrf_regs->tx_addr);
    // transfer(RF_SETUP, &nrf_regs->rf_setup, 1, write_register);
    // transfer(RF_SETUP, (char[1]) {0x0}, 1, read_register);
    // if (receive_buffer[1] != 6)
    //     return 0;
    return 1;
}

void nrf_transmit(char* payload, unsigned int payload_length)
{

}

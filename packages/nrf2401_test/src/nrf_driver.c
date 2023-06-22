#include "nrf24l01.h"
#include "nrf_driver.h"
#include "gpio.h"

Nrf24l01Registers_t current_nrf_config;
GpioObject_t gpio_pa5_ce;

void nrf_power_off()
{
    clear_bit_nrf_register(CONFIG, 1);
}

void nrf_power_on()
{
    set_bit_nrf_register(CONFIG, 1);
    // transfer(CONFIG, (char[1]) {0x2}, 1, WriteRegister);

    // // settle >1,5 msec
    // // sleep(2);

    // transfer(CONFIG, (char[1]) {0x2}, 1, ReadRegister);
    // nrf24l01_regs.config = receive_buffer[0];
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

void clear_ir_maxrt_flag(void)
{
    set_bit_nrf_register(STATUS, 4);
    // transfer(STATUS, (char[1]) {0x7}, 1, ReadRegister); 
    // char new_data[1] = {(receive_buffer[0] | (1 << 4))}; 
    // transfer(STATUS, new_data, 1, WriteRegister); 
}

char configure_device(Nrf24l01Registers_t* nrf_regs, __attribute__((unused)) OperatingMode_t mode)
{
    replace_nrf_register(RF_SETUP, nrf_regs->rf_setup);
    replace_nrf_register(EN_AA, nrf_regs->en_aa);
    set_nrf_register_long(TX_ADDR, nrf_regs->tx_addr);
    // transfer(RF_SETUP, &nrf_regs->rf_setup, 1, WriteRegister);
    // transfer(RF_SETUP, (char[1]) {0x0}, 1, ReadRegister);
    // if (receive_buffer[1] != 6)
    //     return 0;
    return 1;
}



void unset_ce()
{
    gpio_pa5_ce.pin = 5;
    gpio_pa5_ce.port = 'A';
    set_pin_off(&gpio_pa5_ce);
}

void set_ce()
{
    gpio_pa5_ce.pin = 5;
    gpio_pa5_ce.port = 'A';
    set_pin_on(&gpio_pa5_ce);
}


void nrf_transmit(char* payload, unsigned int payload_length)
{
    transfer(-1, (char[1]) {0}, 0, FlushTX);
    clear_ir_maxrt_flag();
    nrf_power_on();
    transfer(-1, payload, payload_length, WTxPayload);

    set_ce();

    for (int i = 0; i < 2000; i++)
    {
        // @todo replace with sleep as soon as fw is updated
    }
    
    unset_ce();


}

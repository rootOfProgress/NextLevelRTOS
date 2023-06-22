#include "gpio.h"
#include "test.h"
#include "spi.h"
#include "nrf24l01.h"
#include "nrf_driver.h"
#include "rcc.h"
#include "uart.h"
#include "os_api.h"

#define SV_YIELD_TASK __asm volatile ("mov r6, 2\n" \
                                  "svc 0\n")


#define READ_REGISTER(addr)     (*(volatile unsigned int *) (addr))
#define WRITE_REGISTER(addr, val) ((*(volatile unsigned int *) (addr)) = (unsigned int) (val))


char receive_buffer[RX_BUFFER_SIZE];
char tx_buffer[TX_BUFFER_SIZE];
Nrf24l01Registers_t nrf24l01_regs;
GpioObject_t gpio_pa5_ce;

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



void clear_ir_flag()
{
    transfer(STATUS, (char[1]) {0x7}, 1, read_register); 
    char new_data[1] = {(receive_buffer[0] | (1 << 4))};
    transfer(STATUS, new_data, 1, write_register); 
}

void send_stuff()
{
    // transfer(FLUSH_TX, (char[1]) {0}, 1, write_register);
    clear_ir_flag();
    // sleep(5);
    nrf_power_on();
    
    char *tx_payload = "fragezeichenfragezeichenantwort\n";
    transfer(0, tx_payload, 32, w_tx_payload);
    set_ce();

    // sleep(10);
    unset_ce();
}


void getconfig()
{
    char empty = 0;
    transfer(RF_SETUP, &empty, sizeof(char), read_register);
    nrf24l01_regs.rf_setup = receive_buffer[1];

    // validate
    transfer(SETUP_AW, &empty, sizeof(char), read_register);
    nrf24l01_regs.setup_aw = receive_buffer[1];

    transfer(RF_CH, &empty, sizeof(char), read_register);
    nrf24l01_regs.rf_ch = receive_buffer[1];

    // validate
    transfer(TX_ADDR, (char[5]) {0,0,0,0,0}, 5, read_register);
    nrf24l01_regs.tx_addr[0] = receive_buffer[0];
    nrf24l01_regs.tx_addr[1] = receive_buffer[1];
    nrf24l01_regs.tx_addr[2] = receive_buffer[2];
    nrf24l01_regs.tx_addr[3] = receive_buffer[3];
    nrf24l01_regs.tx_addr[4] = receive_buffer[4];
    
    transfer(STATUS, &empty, sizeof(char), read_register);
    nrf24l01_regs.status = receive_buffer[1];    

    // validate
    transfer(FIFO_STATUS, &empty, sizeof(char), read_register);
    nrf24l01_regs.fifo_status = receive_buffer[1];
}



void init_tx()
{
    
    transfer(EN_AA, (char[1]) {0x0}, 1, write_register);

    transfer(RF_SETUP, (char[1]) {0x6}, 1, write_register);
    
    // // validate
    // transfer(RF_SETUP, (char[1]) {0x0}, 1, read_register);
    // nrf24l01_regs.rf_setup = receive_buffer[0];

    // /************ 5 byte address width ***************/
    // char payload_aw[1] = {0x3};
    // transfer(SETUP_AW, payload_aw, sizeof(payload_aw) / sizeof(payload_aw[0]), write_register);

    // // validate
    // transfer(SETUP_AW, (char[1]) {0x0}, 1, read_register);
    // nrf24l01_regs.setup_aw = receive_buffer[0];

    // /************ Channel 2 ***************/
    // transfer(RF_CH, (char[1]) {0x2}, 1, write_register);

    // // validate
    // transfer(RF_CH, (char[1]) {0x0}, 1, read_register);
    // nrf24l01_regs.rf_ch = receive_buffer[0];

    // /************ TxAddr 0xcccecccecc ***************/
    // char payload_tx_addr[5] = {0xCC, 0xCE, 0xCC, 0xCE, 0xCC};
    // transfer(TX_ADDR, payload_tx_addr, sizeof(payload_tx_addr) / sizeof(payload_tx_addr[0]), write_register);

    // // validate
    // transfer(TX_ADDR, (char[5]) {0,0,0,0,0}, 5, read_register);
    // nrf24l01_regs.tx_addr[0] = receive_buffer[0];
    // nrf24l01_regs.tx_addr[1] = receive_buffer[1];
    // nrf24l01_regs.tx_addr[2] = receive_buffer[2];
    // nrf24l01_regs.tx_addr[3] = receive_buffer[3];
    // nrf24l01_regs.tx_addr[4] = receive_buffer[4];
    
    // transfer(STATUS, (char[1]) {0x0}, 1, read_register);
    // nrf24l01_regs.status = receive_buffer[0];
}


int __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{   
    init_spi();


    if (0)
        self_check();

    memset_byte((void*) &receive_buffer, sizeof(receive_buffer), 0x0);
    memset_byte((void*) &current_nrf_config, sizeof(current_nrf_config), 0x0);
    
    
    Nrf24l01Registers_t nrf_registers;

    /************ Disable AA ***************/
    nrf_registers.en_aa = 0;

    /************ 1Mbps data rate, 0dBm ***************/
    nrf_registers.rf_setup = 6;

    /************ 5 byte address width ***************/
    nrf_registers.setup_aw = 3;
    
    /************ Channel 2 ***************/
    nrf_registers.rf_ch = 2;

    char tx[5] = {0xCC, 0xCE, 0xCC, 0xCE, 0xCC};
    for (unsigned int i = 0; i < sizeof(tx)/sizeof(char); i++)
        nrf_registers.tx_addr[i] = tx[i];

    configure_device(&nrf_registers, MASTER);
    get_nrf_config();

    print((void*) &current_nrf_config, sizeof(Nrf24l01Registers_t));
    // init_tx();
    
    
    return 0;
}

#include "fifo.h"
#include "gpio.h"
#include "spi.h"
#include "nrf24l01.h"
#include "rcc.h"
#include "../../include/os_api.h"

#define SV_YIELD_TASK __asm volatile ("mov r6, 2\n" \
                                  "svc 0\n")


#define READ_REGISTER(addr)     (*(volatile unsigned int *) (addr))
#define WRITE_REGISTER(addr, val) ((*(volatile unsigned int *) (addr)) = (unsigned int) (val))

char receive_buffer[35];
nrf24l01_registers_t nrf24l01_regs;

void unset_ce()
{
    GpioObject_t t;
    t.pin = 5;
    t.port = 'A';
    set_pin_off(&t);
}

void set_ce()
{
    GpioObject_t t;
    t.pin = 5;
    t.port = 'A';
    set_pin_on(&t);
}

void transfer(char target_register, char *data, unsigned int length, transferType_t t) 
{
    char preamble;
    switch (t)
    {
    case read_register:
        preamble = target_register;
        break;
    case write_register:
        preamble = ((char) (1 << 5) | target_register);
        break;
    // don't care
    case r_rx_payload:
    case w_tx_payload:
        preamble = W_TX_PAYLOAD;
        break;
    default:
        return;
    }
    spi_write(preamble, data, length, receive_buffer);
}

void clear_ir_flag()
{
    transfer(STATUS, (char[1]) {0x7}, 1, read_register); 
    char new_data[1] = {(receive_buffer[0] | (1 << 4))};
    transfer(STATUS, new_data, 1, write_register); 
}

void send_stuff()
{
    transfer(FLUSH_TX, (char[1]) {0}, 1, write_register);
    clear_ir_flag();
    spin(1000);
    power_on();
    
    char *tx_payload = "fragezeichenfragezeichenantwort\n";
    transfer(0, tx_payload, 32, w_tx_payload);
    set_ce();

    spin(2000);
    unset_ce();
}

void spin(int t)
{
    for (int i = 0; i < t; i++) {}
}

void power_off()
{
    // char conf[1] = {0x0};
    transfer(CONFIG, (char[1]) {0}, 1, write_register);

    // settle >1,5 msec
    spin(150);

    transfer(CONFIG, (char[1]) {0}, 1, read_register);
    nrf24l01_regs.config = receive_buffer[0];
}

void power_on()
{
    transfer(CONFIG, (char[1]) {0x2}, 1, write_register);

    // settle >1,5 msec
    spin(150);

    transfer(CONFIG, (char[1]) {0x2}, 1, read_register);
    nrf24l01_regs.config = receive_buffer[0];
}

void getconfig()
{
    char dummy[1] = {0};
    transfer(RF_SETUP, (char[1]) {0}, 1, read_register);
    nrf24l01_regs.rf_setup = receive_buffer[0];

    // validate
    transfer(SETUP_AW, (char[1]) {0}, 1, read_register);
    nrf24l01_regs.setup_aw = receive_buffer[0];

    transfer(RF_CH, (char[1]) {0}, 1, read_register);
    nrf24l01_regs.rf_ch = receive_buffer[0];

    // validate
    transfer(TX_ADDR, (char[5]) {0,0,0,0,0}, 5, read_register);
    nrf24l01_regs.tx_addr[0] = receive_buffer[0];
    nrf24l01_regs.tx_addr[1] = receive_buffer[1];
    nrf24l01_regs.tx_addr[2] = receive_buffer[2];
    nrf24l01_regs.tx_addr[3] = receive_buffer[3];
    nrf24l01_regs.tx_addr[4] = receive_buffer[4];
    
    transfer(STATUS, dummy, sizeof(dummy) / sizeof(dummy[0]), read_register);
    nrf24l01_regs.status = receive_buffer[0];    

    // validate
    transfer(FIFO_STATUS, dummy, sizeof(dummy) / sizeof(dummy[0]), read_register);
    nrf24l01_regs.fifo_status = receive_buffer[0];
}

void init_rx()
{
    /************ Disable AA ***************/
    transfer(EN_AA, (char[1]) {0x0}, 1, write_register);

    /************ 1Mbps data rate, 0dBm ***************/
    transfer(RF_SETUP, (char[1]) {0x6}, 1, write_register);
    
    // validate
    transfer(RF_SETUP, (char[1]) {0x0}, 1, read_register);
    nrf24l01_regs.rf_setup = receive_buffer[0];

    /************ 5 byte address width ***************/
    char payload_aw[1] = {0x3};
    transfer(SETUP_AW, payload_aw, sizeof(payload_aw) / sizeof(payload_aw[0]), write_register);

    // validate
    transfer(SETUP_AW, (char[1]) {0x0}, 1, read_register);
    nrf24l01_regs.setup_aw = receive_buffer[0];

    /************ Channel 2 ***************/
    transfer(RF_CH, (char[1]) {0x2}, 1, write_register);

    // validate
    transfer(RF_CH, (char[1]) {0x0}, 1, read_register);
    nrf24l01_regs.rf_ch = receive_buffer[0];

    /************ RxAddr 0xcccecccecc ***************/
    char payload_tx_addr[5] = {0xCC, 0xCE, 0xCC, 0xCE, 0xCC};
    transfer(TX_ADDR, payload_tx_addr, sizeof(payload_tx_addr) / sizeof(payload_tx_addr[0]), write_register);

    // validate
    transfer(TX_ADDR, (char[5]) {0,0,0,0,0}, 5, read_register);
    nrf24l01_regs.tx_addr[0] = receive_buffer[0];
    nrf24l01_regs.tx_addr[1] = receive_buffer[1];
    nrf24l01_regs.tx_addr[2] = receive_buffer[2];
    nrf24l01_regs.tx_addr[3] = receive_buffer[3];
    nrf24l01_regs.tx_addr[4] = receive_buffer[4];
    
    transfer(STATUS, (char[1]) {0x0}, 1, read_register);
    nrf24l01_regs.status = receive_buffer[0];
}

void init_tx()
{
    /************ Disable AA ***************/
    transfer(EN_AA, (char[1]) {0x0}, 1, write_register);

    /************ 1Mbps data rate, 0dBm ***************/
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

    //  nRF24L01+ must be in a standby or power down mode 
    // before writing to the configuration registers
    // 8.3.2 page 49
    // BREAK;
    // if (0)
    // {
    //     init_spi();
    // }
    // else 
    // {
    //     init_tx();
    //     spin(150);
    //     send_stuff();
    //     SV_YIELD_TASK;
    // }
    // DebugConfig_t init;
    // init.is_tx = 0;
    // init.needs_init = 1;
    init_spi();
    init_tx();
    // if (init.needs_init)
    // {
    //     init_spi();
    // }
    // else
    // {
    //     spin(150);
    //     switch (init.is_tx)
    //     {
    //     case 0:
    //         init_rx();
    //         // poll_rx();
    //         break;
    //     case 1:
    //         init_tx();
    //         // send_stuff();
    //     default:
    //         break;
    //     }
    // }
    
    return 0;
}

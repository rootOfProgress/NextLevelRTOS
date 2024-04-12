#include "gpio/gpio.h"
#include "test.h"
#include "spi.h"
#include "nrf24l01.h"
#include "rcc.h"
#include "uart.h"
#include "os_api.h"

#define SV_YIELD_TASK __asm volatile ("mov r6, 2\n" \
                                  "svc 0\n")


#define READ_REGISTER(addr)     (*(volatile unsigned int *) (addr))
#define WRITE_REGISTER(addr, val) ((*(volatile unsigned int *) (addr)) = (unsigned int) (val))


// #define SHOCKY
// char receive_buffer[RX_BUFFER_SIZE];
// char tx_buffer[TX_BUFFER_SIZE];
Nrf24l01Registers_t nrf24l01_regs;


void apply_nrf_config(Nrf24l01Registers_t *nrf_registers)
{

    /************ Disable AA ***************/
    #ifdef SHOCKY
    nrf_registers->en_aa = 1; // ENAA_P0
    nrf_registers->setup_retr = 7;
    nrf_registers->en_rxaddr = 1;
    #else
    nrf_registers->en_aa = 0;
    #endif
    /************ 1Mbps data rate, 0dBm ***************/
    nrf_registers->rf_setup = 6;

    /************ 5 byte address width ***************/
    nrf_registers->setup_aw = 3;
    
    /************ Channel 3 ***************/
    nrf_registers->rf_ch = 3;

    nrf_registers->config = 0; // en_crc

    // LSB is written first, will result in bfcecccecc
    // char tx[5] = {0xCC, 0xCE, 0xCC, 0xCE, 0xBF};
    // char tx[5] = {0x9A, 0x78, 0x56, 0x34, 0x12};
    // char tx[5] = {0xc6, 0xc6, 0xc6, 0xc6, 0xc6};
    char tx[5] = {0xc5, 0xc5, 0xc5, 0xc5, 0xc5};
    for (unsigned int i = 0; i < sizeof(tx)/sizeof(char); i++)
        nrf_registers->tx_addr[i] = tx[i];
    // for (int i = sizeof(tx)/sizeof(char) - 1; i >= 0 ; i--)

    // char rx_p0[5] = {0xc5, 0xc5, 0xc5, 0xc5, 0xc5};
    char rx_p0[5] = {0xa1, 0xa1, 0xa1, 0xa1, 0xa1};
    for (unsigned int i = 0; i < sizeof(rx_p0)/sizeof(char); i++)
        nrf_registers->rx_addr_p0[i] = rx_p0[i];
    // for (int i = sizeof(rx_p0)/sizeof(char) - 1; i >= 0; i--)
}

int __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{   
    // now done in driver!
    // init_spi();
    
   
    Nrf24l01Registers_t nrf_registers;
    apply_nrf_config(&nrf_registers);

    configure_device(&nrf_registers, MASTER);
    
    #ifdef SHOCKY
    
    #endif
    char* p = "abcdefghi";
    // unsigned int p = 0xAABBCCDD;

    nrf_transmit(p, 6);
    
    return 0;
}

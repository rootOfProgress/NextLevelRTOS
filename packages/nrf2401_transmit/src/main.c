#include "gpio/gpio.h"
#include "test.h"
#include "spi.h"
#include "nrf24l01.h"
#include "rcc.h"
#include "uart.h"
#include "os_api.h"
#include "crc.h"

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
    nrf_registers->en_aa = 0x3F; // ENAA_P0
    nrf_registers->setup_retr = 7;
    nrf_registers->en_rxaddr = 0x3F;
    #else
    nrf_registers->en_aa = 0;
    nrf_registers->en_rxaddr = 0;

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
    for (unsigned int i = 0; i < sizeof(tx)/sizeof(char); i++)
        nrf_registers->tx_addr[i] = tx[i];

    char rx_p0[5] = {0xc5, 0xc5, 0xc5, 0xc5, 0xc5};
    // char rx_p0[5] = {0xa1, 0xa1, 0xa1, 0xa1, 0xa1};
    for (unsigned int i = 0; i < sizeof(rx_p0)/sizeof(char); i++)
        nrf_registers->rx_addr_p0[i] = rx_p0[i];
    // for (int i = sizeof(rx_p0)/sizeof(char) - 1; i >= 0; i--)
}

int __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{   
    // now done in driver!
    // init_spi();
    
    crc_activate();
   
    Nrf24l01Registers_t nrf_registers;
    apply_nrf_config(&nrf_registers);
    // config.

    configure_device(&nrf_registers, MASTER);
    
    #ifdef SHOCKY
    
    #endif
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

    char* t = "llkj;vmdd";
    char* r = "ipoijvmdd";
    
    load_tx_buffer(32, outBuffer);
    // load_tx_buffer(6, t);
    // load_tx_buffer(6, r);
    // transmit_single_package();
    transmit_all_packages();
    return 0;
}

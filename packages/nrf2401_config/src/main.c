#include "gpio.h"
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



Nrf24l01Registers_t nrf24l01_regs;


int __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{   
    // init_spi();

    Nrf24l01Registers_t current_nrf_config;

    memset_byte((void*) &current_nrf_config, sizeof(current_nrf_config), 0x0);
    
    
    get_nrf_config(&current_nrf_config);
    print((void*) &current_nrf_config, sizeof(Nrf24l01Registers_t));
    
    return 0;
}

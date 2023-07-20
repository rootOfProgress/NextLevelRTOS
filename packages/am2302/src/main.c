#include "gpio.h"
#include "spi.h"
#include "rcc.h"
#include "uart.h"
#include "os_api.h"
#include "am2302.h"

#define SV_YIELD_TASK __asm volatile ("mov r6, 2\n" \
                                  "svc 0\n")


#define READ_REGISTER(addr)     (*(volatile unsigned int *) (addr))
#define WRITE_REGISTER(addr, val) ((*(volatile unsigned int *) (addr)) = (unsigned int) (val))




int __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{   
    // GpioObject_t t;
    // t.pin = 0;
    // asm("bkpt");
    // t.port = 'A';
    am2302_init_peripherials(0, 'A');
    Am2302Readings_t readings;
    am2302_do_measurement(&readings);
    asm("bkpt");
    return 0;
}

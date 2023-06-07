#define SV_YIELD_TASK __asm volatile ("mov r6, 2\n" \
                                  "svc 0\n")

#define SV_PRINT __asm volatile ("mov r6, 1\n" \
                                  "svc 0\n")

#include "os_api.h"
#include "rcc.h"
#include "gpio.h"

static unsigned int start_measurement = 0b010;
static unsigned int stop_measurement = 0b111;

void spin()
{

        
}

void  __attribute__((__noipa__))  __attribute__((optimize("O0"))) benchmark(GpioObject_t* obj)
{
    unsigned int* (*baz)(unsigned int) = (unsigned int* (*)(unsigned int size)) (0x8001621);
    set_pin_bulk(obj, 2);
    baz(32);
    set_pin_bulk(obj, 5);
    for (int i = 0; i < 2000; i++)
    {
        /* code */
    }    
}

void __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{
    // PB 0, 1, 2 used
    // Memory Measurement Start: 010
    // Memory Measurement Stop: 101
    
    GpioObject_t gpio_b;
    gpio_b.port = 'B';
    gpio_b.pin = 0;

    init_gpio(&gpio_b);
    set_moder(&gpio_b, GeneralPurposeOutputMode);

    gpio_b.pin = 1;
    set_moder(&gpio_b, GeneralPurposeOutputMode);
    set_pupdr(&gpio_b, Nothing);

    gpio_b.pin = 2;
    set_moder(&gpio_b, GeneralPurposeOutputMode);
    // asm("bkpt");
    set_pin_bulk(&gpio_b, 0);
    for (int i = 0; i < 40; i++)
    {
        benchmark(&gpio_b);
    }
    
    
}

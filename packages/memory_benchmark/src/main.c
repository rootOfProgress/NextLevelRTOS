#define SV_YIELD_TASK __asm volatile ("mov r6, 2\n" \
                                  "svc 0\n")

#define SV_PRINT __asm volatile ("mov r6, 1\n" \
                                  "svc 0\n")

#include "os_api.h"
#include "rcc.h"
#include "gpio.h"
#include "uart.h"
#include "tim2_5.h"

static unsigned int start_measurement = 0b010;
static unsigned int stop_measurement = 0b111;

void spin()
{

        
}
typedef struct MeasurementResults {
    unsigned int results[32]; 
} MeasurementResults_t;

void  __attribute__((__noipa__))  __attribute__((optimize("O0"))) benchmark(GpioObject_t* obj, unsigned int alloc_size)
{
    unsigned int* (*baz)(unsigned int) = (unsigned int* (*)(unsigned int size)) (0x8001621);
    set_pin_bulk(obj, 2);
    baz(alloc_size);
    set_pin_bulk(obj, 5);
    for (int i = 0; i < 2000; i++)
    {
        /* code */
    }    
}

void __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{
    MeasurementResults_t measurements;
    timer_init(2, 1, (char[4]) {0,0,0,0}, 1);
    unsigned int* (*baz)(unsigned int) = (unsigned int* (*)(unsigned int size)) (0x8001629);

    // while (timer_read_counter(2) < 0xF4240)
    // {
    //     /* code */
    // }
    
    // PB 0, 1, 2 used
    // Memory Measurement Start: 010
    // Memory Measurement Stop: 101
    unsigned int rand1 = READ_REGISTER(0x20006004);
    unsigned int rand2 = READ_REGISTER(0x200060C4);
    unsigned int rand3 = READ_REGISTER(0x200062C4);
    unsigned int rand4 = READ_REGISTER(0x200064C4);

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
    set_pin_bulk(&gpio_b, 0);

    for (int i = 0; i < 32; i++)
    {
        timer_flush_counter(2);
        timer_start(2);
        unsigned int t = baz(4);
        timer_stop(2);
        measurements.results[i] = timer_read_counter(2);
    }
    // for (int i = 0x20002CC4; i < 0x20002CD4; i += 0x04)
    // {
    //     unsigned int r1 = READ_REGISTER(i);
        
    //     benchmark(&gpio_b, r1 & 0xFF);
    //     benchmark(&gpio_b, (r1 >> 8) & 0xFF);
    //     benchmark(&gpio_b, (r1 >> 16) & 0xFF);
    //     benchmark(&gpio_b, (r1 >> 24) & 0xFF);
    // }
    
    // measurements.results[0] = 66789;
    print(&measurements.results, 32 * sizeof(int));
}

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

void __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{
    MeasurementResults_t measurements;
    timer_init(2, 1, (char[4]) {0,0,0,0}, 1);

    for (int j = 0; j < 32; j++)
    {
        timer_flush_counter(2);
        timer_start(2);
        SV_YIELD_TASK;
        timer_stop(2);
        measurements.results[j] = timer_read_counter(2);  
    }
        
    print(&measurements.results, 32 * sizeof(int));
}

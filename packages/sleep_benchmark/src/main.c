#define SV_YIELD_TASK __asm volatile ("mov r6, 2\n" \
                                  "svc 0\n")

#define SV_PRINT __asm volatile ("mov r6, 1\n" \
                                  "svc 0\n")

#include "os_api.h"
#include "rcc.h"
#include "uart.h"
#include "tim2_5.h"
#include "addresses.h"
#include "parameters.h"

typedef struct MeasurementResults {
    unsigned int results[32]; 
} MeasurementResults_t;

int __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{
    MeasurementResults_t measurements;
    timer_init(2, 1, (unsigned int[4]) {0,0,0,0}, 1);
    asm("bkpt");
    for (int j = 0; j < 4; j++)
    {
        timer_flush_counter(2);
        timer_start(2);
        sleep(sleep_times[j]);
        timer_stop(2);
        measurements.results[j] = timer_read_counter(2);  
    }
        
    print((char*) &measurements.results, 4 * sizeof(int));
    return 0;
}

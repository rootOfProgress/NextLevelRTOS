#define SV_YIELD_TASK __asm volatile ("mov r6, 2\n" \
                                  "svc 0\n")

#define SV_PRINT __asm volatile ("mov r6, 1\n" \
                                  "svc 0\n")

#include "os_api.h"
#include "rcc.h"
#include "gpio.h"
#include "uart.h"
#include "tim2_5.h"
#include "parameters.h"

typedef struct MeasurementResults {
    unsigned int results[32]; 
} MeasurementResults_t;

void  __attribute__((__noipa__))  __attribute__((optimize("O0"))) benchmark(unsigned int alloc_size, MeasurementResults_t *measurements, unsigned int round)
{
    timer_flush_counter(2);
    timer_start(2);
    unsigned int *t = allocate(alloc_size);
    timer_stop(2);
    deallocate(t);
    measurements->results[round] = timer_read_counter(2);  
}

void __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{
    MeasurementResults_t measurements;
    timer_init(2, 1, (unsigned int[4]) {0,0,0,0}, 1);

    for (int j = 0; j < 32; j++)
    {
        benchmark(alloc_chunks[j], &measurements, j);
    }
    print((char*) &measurements.results, 32 * sizeof(int));
}

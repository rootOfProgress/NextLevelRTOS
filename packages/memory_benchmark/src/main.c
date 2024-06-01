#define SV_YIELD_TASK __asm volatile ("mov r6, 2\n" \
                                  "svc 0\n")

#define SV_PRINT __asm volatile ("mov r6, 1\n" \
                                  "svc 0\n")

#include "os_api.h"
#include "rcc.h"
#include "uart.h"
#include "tim2_5.h"
#include "parameters.h"

typedef struct MeasurementResults {
    unsigned int results[32]; 
} MeasurementResults_t;

void  __attribute__((__noipa__))  __attribute__((optimize("O0"))) benchmark(unsigned int alloc_size, MeasurementResults_t *measurements, unsigned int round)
{
    unsigned int tStart, tEnd; 
    tStart = read_timer();
    unsigned int *t = allocate(alloc_size);
    tEnd = read_timer();
    deallocate(t);
    measurements->results[round] = tEnd - tStart;
}

int __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{
    MeasurementResults_t measurements;
    for (int j = 0; j < 32; j++)
    {
        benchmark(alloc_chunks[j], &measurements, j);
    }
    print((char*) &measurements.results, 32 * sizeof(int));
    return 0;
}

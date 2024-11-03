#define SV_YIELD_TASK __asm volatile ("mov r6, 2\n" \
                                  "svc 0\n")

#define SV_PRINT __asm volatile ("mov r6, 1\n" \
                                  "svc 0\n")

#include "os_api.h"
#include "rcc.h"
#include "uart.h"
#include "tim2_5.h"


typedef struct MeasurementResults {
    unsigned int results[32]; 
} MeasurementResults_t;

int __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{
    MeasurementResults_t measurements;
    unsigned int tStart, tEnd; 
    for (int j = 0; j < 32; j++)
    {
        tStart = read_timer();
        SV_YIELD_TASK;
        tEnd = read_timer();
        measurements.results[j] = tEnd - tStart;
    }
        
    print((char*) &measurements.results, 32 * sizeof(int));

    return 0;
}

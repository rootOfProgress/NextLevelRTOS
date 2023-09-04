#define SV_YIELD_TASK __asm volatile ("mov r6, 2\n" \
                                  "svc 0\n")

#define SV_PRINT __asm volatile ("mov r6, 1\n" \
                                  "svc 0\n")

#include "os_api.h"
#include "rcc.h"
#include "uart.h"
#include "tim2_5.h"
#include "parameters.h"

static unsigned int cnt_var;
unsigned int mutex;

typedef struct MeasurementResults {
    char Subtest002_000_mutex; 
    char reserved[31]; 
} MeasurementResults_t;

void counter_unprotected(void)
{
    while (1)
    {
        if (cnt_var == 200000000)
        {
            return;
        }
        cnt_var++;
    }
}

void counter_protected(void)
{
    while (1)
    {
        lock_mutex((void*) &mutex);
        if (cnt_var == 200000000)
        {
            release_mutex((void*) &mutex);
            return;
        }
        cnt_var++;
        release_mutex((void*) &mutex);
    }
}


int __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{
    MeasurementResults_t measurements;
    memset_byte((void*) &measurements, sizeof(MeasurementResults_t), 0);
    cnt_var = 0;
    mutex = 0;

    
    void* dummy = allocate(4);
    if (((unsigned int) dummy - (unsigned int) t.address) == 4)
    {
        measurements.Subtest002_004_alloc_gap = 1;
    }

    print((char*) &measurements, sizeof(MeasurementResults_t));
    return 0;
}

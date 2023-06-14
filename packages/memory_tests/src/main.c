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
    unsigned int alloc_negative : 1, alloc_outofrange : 1, : 30; 
} MeasurementResults_t;


void __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{
    MeasurementResults_t measurements;
    measurements.alloc_negative = 0;
    

    print(&measurements, sizeof(int));
}

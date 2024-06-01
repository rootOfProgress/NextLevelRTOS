#define SV_YIELD_TASK __asm volatile ("mov r6, 2\n" \
                                  "svc 0\n")

#define SV_PRINT __asm volatile ("mov r6, 1\n" \
                                  "svc 0\n")

#include "os_api.h"
#include "rcc.h"
#include "uart.h"
#include "tim2_5.h"
#include "parameters.h"

static unsigned char loop_was_triggered;

typedef struct MeasurementResults {
    char Subtest006_000_append_loop_task;
    char reserved[31];
} MeasurementResults_t;

void __attribute__((__noipa__))  __attribute__((optimize("O0"))) loop(void)
{
    while (1)
    {
        loop_was_triggered = 1;
    }
   
}

char Subtest006_000_append_loop_task(void)
{
    create_task(&loop, 0);
    SV_YIELD_TASK;
    return 1;
}

int __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{
    MeasurementResults_t measurements;
    memset_byte((void*) &measurements, sizeof(MeasurementResults_t), 0);

    loop_was_triggered = 0;
    measurements.Subtest006_000_append_loop_task = Subtest006_000_append_loop_task() & loop_was_triggered;
    

    print((char*) &measurements, sizeof(MeasurementResults_t));
    return 0;
}

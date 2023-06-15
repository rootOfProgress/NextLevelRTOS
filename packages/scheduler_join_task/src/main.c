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
    char Subtest005_000_join_task;
    char reserverd[31];
} MeasurementResults_t;

static unsigned int t0;
static unsigned int t1;
static unsigned int t2;
static unsigned int t3;
static unsigned int t4;

void __attribute__((__noipa__))  __attribute__((optimize("O0"))) task3(void)
{
}

void __attribute__((__noipa__))  __attribute__((optimize("O0"))) task2(void)
{
}

void __attribute__((__noipa__))  __attribute__((optimize("O0"))) task1(void)
{
    t0 = timer_read_counter(2);
}

void __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{
    MeasurementResults_t measurements;
    memset_byte((void*) &measurements, sizeof(MeasurementResults_t), 0);
    timer_init(2, 1, (char[4]) {0,0,0,0}, 1);

    int pid_t1 = create_task(&task1, 0);
    timer_start(2);
    join_task(pid_t1);
    t1 = timer_read_counter(2);
    timer_stop(2);

    if (t1 > t0)
        measurements.Subtest005_000_join_task = 1;

    print(&measurements, sizeof(int));
}

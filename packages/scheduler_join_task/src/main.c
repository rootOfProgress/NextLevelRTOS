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
    char Subtest005_001_join_multiple_tasks;
    char Subtest005_002_no_join;
    char Subtest005_003_join_nested_tasks;
    char reserverd[28];
} MeasurementResults_t;

static unsigned int t0;
static unsigned int t1;
static unsigned int t2;
static unsigned int t3;
static unsigned int t4;


void __attribute__((__noipa__))  __attribute__((optimize("O0"))) task2_nested(void)
{
    t2 = timer_read_counter(2);
}

void __attribute__((__noipa__))  __attribute__((optimize("O0"))) task1_nested(void)
{
    t1 = timer_read_counter(2);
    int pid_of_t2_nested = create_task(&task2_nested, 0);
    join_task(pid_of_t2_nested);
}

void __attribute__((__noipa__))  __attribute__((optimize("O0"))) task3(void)
{
    t3 = timer_read_counter(2);
}

void __attribute__((__noipa__))  __attribute__((optimize("O0"))) task2(void)
{
    t2 = timer_read_counter(2);
}

void __attribute__((__noipa__))  __attribute__((optimize("O0"))) task1(void)
{
    t0 = timer_read_counter(2);
}

char Subtest005_003_join_nested_tasks(void)
{
    int pid_of_t1_nested = create_task(&task1_nested, 0);
    char result = 1;

    timer_start(2);
    t0 = timer_read_counter(2);
    join_task(pid_of_t1_nested);
    
    if (t2 > t1 || t1 > t0 || t2 > t0)
        return 0;

    return result;
}

char Subtest005_002_no_join(void)
{
    create_task(&task1, 0);
    create_task(&task2, 0);
    create_task(&task3, 0);
    char result = 1;

    if (t0 != 0 || t1 != 0 || t2 != 0)
        result = 0;
    return result;
}

char Subtest005_001_join_multiple_tasks(void)
{
    int pid_t1 = create_task(&task1, 0);
    int pid_t2 = create_task(&task2, 0);
    int pid_t3 = create_task(&task3, 0);
    char result = 1;

    timer_start(2);

    join_task(pid_t1);
    unsigned int join_t1 = timer_read_counter(2);
    if (join_t1 < t0)
        result = 0;

    join_task(pid_t2);
    unsigned int join_t2 = timer_read_counter(2);
    if (join_t2 < t2)
        result = 0;

    join_task(pid_t3);
    unsigned int join_t3 = timer_read_counter(2);
    if (join_t3 < t3)
        result = 0;

    timer_stop(2);

    return result;
}

char Subtest005_000_join_task(void)
{
    int pid_t1 = create_task(&task1, 0);
    timer_start(2);
    join_task(pid_t1);
    t1 = timer_read_counter(2);
    timer_stop(2);
    if (t1 > t0)
        return 1;
    return 0;
}

void flush_times()
{
    t0 = 0;
    t1 = 0;
    t2 = 0;
    t3 = 0;
    t4 = 0;
}

int __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{
    MeasurementResults_t measurements;
    memset_byte((void*) &measurements, sizeof(MeasurementResults_t), 0);
    timer_init(2, 1, (unsigned int[4]) {0,0,0,0}, 1);

    measurements.Subtest005_000_join_task = Subtest005_000_join_task();
    flush_times();
    measurements.Subtest005_001_join_multiple_tasks = Subtest005_001_join_multiple_tasks();
    flush_times();
    measurements.Subtest005_002_no_join = Subtest005_002_no_join();
    flush_times();
    measurements.Subtest005_003_join_nested_tasks = Subtest005_003_join_nested_tasks();
    

    print((char*) &measurements, sizeof(MeasurementResults_t));
}

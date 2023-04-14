#include "tim2_5.h"
#include "process/scheduler.h"

void __attribute__((optimize("O0"))) tim3_isr_handler(void)
{
    unsigned tim_sr = get_sr(3);
    if (tim_sr & (1 << 1))
    {
        timer_stop(3);

        // acknowledge ir reception
        set_sr(3, tim_sr & ~(1 << 1));
        wakeup_pid(task_sleep_request.pid_of_sleeping_task);
    }
}
#include "tim2_5.h"
#include "process/scheduler.h"
#include "runtime_types.h"
#include "panic.h"
#include "timer/tim2_5_bitfields.h"

static unsigned int isr_calls = 0;

void __attribute__((optimize("O0"))) tim3_isr_handler(void)
{
    isr_calls++;
    unsigned int tim_sr = timer_get_sr(3);

    timer_stop(3);

    // CC1 Interrupt Triggered
    if (tim_sr & (1 << CC1IF))
    {
        // acknowledge ir reception
        timer_set_sr(TimerForTaskSleep, (tim_sr & ~(1 << CC1IF)) | 1);
        wakeup_pid(task_sleep_request.pid_of_sleeping_task);
    }
    else if (tim_sr & (1 << CC2IF))
    {
        // acknowledge ir reception
        timer_set_sr(TimerForTaskSleep, (tim_sr & ~(1 << CC2IF)) | 1);
        execute_panic();
    }
    else 
    {
        while (1)
        {
            /* code */
        }
        
    }
}

// void __attribute__((optimize("O0"))) tim3_isr_handler(void)
// {
//     unsigned tim_sr = timer_get_sr(3);
//     if (tim_sr & (1 << 1))
//     {
//         timer_stop(3);

//         // acknowledge ir reception
//         timer_set_sr(3, tim_sr & ~(1 << 1));
//         wakeup_pid(task_sleep_request.pid_of_sleeping_task);
//     }
// }
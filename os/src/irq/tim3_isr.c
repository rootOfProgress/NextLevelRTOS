#include "tim2_5.h"
#include "process/scheduler.h"

void __attribute__((optimize("O0"))) tim3_isr_handler(void)
{

  unsigned tim_sr = timer_get_sr(TimerForTaskSleep);
  
  if (tim_sr & (1 << CC1IF))
  {
    timer_set_sr(TimerForTaskSleep, tim_sr & ~(1 << CC1IF));
  }
  else if (tim_sr & (1 << CC2IF))
  {
    timer_set_sr(TimerForTaskSleep, tim_sr & ~(1 << CC2IF));
  }
  else if (tim_sr & (1 << CC3IF))
  {
    timer_set_sr(TimerForTaskSleep, tim_sr & ~(1 << CC3IF));
  }
  else if (tim_sr & (1 << CC4IF))
  {
    timer_set_sr(TimerForTaskSleep, tim_sr & ~(1 << CC4IF));
  }

  if (task_sleep_request.pid_of_sleeping_task != 0)
  {
    wakeup_pid(task_sleep_request.pid_of_sleeping_task);
  }
  return;
}
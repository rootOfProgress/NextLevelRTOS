#include "panic.h"
#include "process/scheduler.h"
#include "runtime_types.h"
#include "tim2_5.h"
#include "lang.h"

static unsigned int exec_panic_calls = 0;
PanicTypes_t panic_state;
unsigned int index;

/* On Phase 1 sec, Off Phase 1 sec */
static unsigned int sched_init_failed[] = {1000, 1000}; 
static unsigned int out_of_memory[] = {5000, 5000}; 

void NO_OPT reset_panic(void)
{
  set_pin_off(&general_error_led);
  process_stats.panic_state = NO_PANIC;
  disable_ccx_ir(TimerForTaskSleep, 2);
}

void NO_OPT set_panic(PanicTypes_t panic_type)
{
    index = 0;
    exec_panic_calls = 0;
    process_stats.panic_state = panic_type;
    timer_flush_counter(TimerForTaskSleep);
    enable_ccx_ir(TimerForTaskSleep, 2);
    execute_panic();
}

void NO_OPT execute_panic(void)
{
  exec_panic_calls++;
  toggle_output_pin(&general_error_led);

  switch (process_stats.panic_state)
  {
  case SCHEDULER_NOT_INITIALIZED:
      set_ccr(TimerForTaskSleep, sched_init_failed[index++ % 2], 2);
      break;
  case OUT_OF_MEMORY:
      set_ccr(TimerForTaskSleep, 500 /* out_of_memory[index++ % 2] */, 2);
      break;
  default:
      break;
  }

  timer_flush_counter(TimerForTaskSleep);
  generate_ue(TimerForTaskSleep);
  timer_start(TimerForTaskSleep);

}
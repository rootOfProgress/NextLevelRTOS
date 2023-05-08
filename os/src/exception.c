#include "exception.h"
#include "lang.h"
#include "hw/cpu.h"
#include "process/task.h"
#include "memory.h"
#include "test.h"
#include "uart_common.h"
#include "uart.h"
#include "process/scheduler.h"

volatile unsigned int svc_number = 0;

void __attribute__((__noipa__)) systick_isr()
{
  save_psp_if_threadmode();

  if (DEBUG && currently_running)
  {
    Tcb_t* tcb_of_current_task = ((Tcb_t*)currently_running->data);
    tcb_of_current_task->lifetime_info[0].lifetime.forced_interrupts++;
    process_stats.num_of_systick_interrupts++;

    if (DEBUG == 2)
    {
        tcb_of_current_task->lifetime_info[0].lifetime.cpu_time += timer_read_counter(TimerForSysLogging); 
    }
  }

  __asm volatile ("mrs %0, psp" : "=r"(((Tcb_t*) task_to_preserve->data)->sp));

  switch_task();
  if (DEBUG == 2)
  {
      timer_flush_counter(TimerForSysLogging);
      timer_start(TimerForSysLogging);
  }
  unsigned int next = ((Tcb_t*) currently_running->data)->sp;

  unsigned int ram_upperbound = mstat.ram_size + RAM_START;

  __asm volatile ("mov r2, %[next_sp]":: [next_sp] "r" (next));
  __asm volatile ("mov r3, %[ram_top]":: [ram_top] "r" (ram_upperbound));

  __asm volatile (
    "ldmfd r2!, {r4-r11}\n"
    "msr psp, r2\n"
    "msr msp, r3\n"
    "bx lr\n"
  );
}

__attribute__((used)) void uprint(volatile unsigned int* transfer_info __attribute__((unused)))
{
  SV_PRINT;
}

__attribute__((used)) void execute_priviledged(unsigned int function_address)
{
  SV_EXEC_PRIV;
}

void __attribute__((optimize("O0"))) kprint(void)
{
  volatile unsigned int general_service_register;
  __asm__("mov %0, r9" : "=r"(general_service_register));

  volatile TransferInfo_t* t = (TransferInfo_t*) general_service_register;
  setup_transfer((char*) t->start_adress, t->length);
}

void __attribute__((optimize("O3"))) svcall_isr()
{
  // r0 must be preserved, it could be used by e.g. disable_systick()
  __asm__("mov r9, r0");

  if (SYSTICK)
    disable_systick();

  if (DEBUG == 2)
  {
    timer_stop(TimerForSysLogging);
    process_stats.num_of_svcalls++;
  }
  
  save_psp_if_threadmode();
  __asm__("mov %0, r6" : "=r"(svc_number));

  switch (svc_number)
  {
  case EXEC_PSP_TASK:
    if (SYSTICK) 
    {
      init_systick(200);
      enable_systick();
    }

    Tcb_t* tcb_of_pid0 = ((Tcb_t*)currently_running->data);
    __asm volatile ("mov r0, %[sp]" :: [sp] "r" (tcb_of_pid0->sp));
    __asm volatile ("ldmia.w  r0!, {r4-r11}");
    __asm volatile ("msr psp, r0");
    __asm volatile ("mov r1, 0xfffffffd");
    __asm volatile ("str r1, [sp, #4]");
    return;
  case PRINT_MSG:
    kprint();
  case YIELD_TASK:
    if (DEBUG && currently_running)
    {
      Tcb_t* tcb_of_current_task = ((Tcb_t*)currently_running->data);
      tcb_of_current_task->lifetime_info[0].lifetime.voluntary_interrupts++;      
    }
    set_pendsv();
    break;
  case STD:
    if (SYSTICK)
    {
      disable_systick();
    }
    restore_psp();
    return;
  case STE:
    if (SYSTICK)
    {
      enable_systick();
    }
    restore_psp();
    return;
  case EXEC_PRIV:
    unsigned int function_adress;
    __asm__("mov %0, r9" : "=r"(function_adress));
    void (*priv_fn)() = (void (*)()) (function_adress | 1);
    priv_fn();
    restore_psp();
    return;    
  case SLEEP:
    // block task
    // set timer
    // yield baby
  default:
    __builtin_unreachable();
    break;
  }
  if (SYSTICK)
    enable_systick();
}
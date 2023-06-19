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
// volatile unsigned int systick_cnt = 0;
// volatile unsigned int logging[32];
void ISR systick_isr()
{
  // disable systick
  ST_DISABLE;

  save_psp_if_threadmode();
  // systick_cnt++;
  if (DEBUG && currently_running)
  {
    // logging[systick_cnt % 32] = (unsigned int) currently_running->data->general.task_info.pid;
    // logging[systick_cnt % 32] = systick_cnt % 32;
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
    // enable systick
    "mov.w	r0, #3758153728\n"
    "ldr	r1, [r0, #16]\n"
    "orr.w	r1, r1, #1\n"
    "str	r1, [r0, #16]\n"  
    "bx lr\n"
  );
}

USED void uprint(UNUSED volatile unsigned int* transfer_info )
{
  SV_PRINT;
}

USED void execute_priviledged(UNUSED unsigned int function_address)
{
  SV_EXEC_PRIV;
}

void NO_OPT kprint(void)
{
  volatile unsigned int general_service_register;
  __asm__("mov %0, r9" : "=r"(general_service_register));

  volatile TransferInfo_t* t = (TransferInfo_t*) general_service_register;
  setup_transfer((char*) t->start_adress, t->length);
}

void __attribute__ ((interrupt("SWI"))) svcall_isr()
{
  // @todo!
  // r0 must be preserved, it could be used by e.g. EXEC_PRIV
  // __asm__("mov r9, r0");

  if (SYSTICK)
  {
    __asm volatile(
        "mov.w	r2, #3758153728\n"
        "ldr	r3, [r2, #16]\n"
        "bic.w	r3, r3, #1\n"
        "str	r3, [r2, #16]\n"
    );
  }
  
  save_psp_if_threadmode();
  __asm__("mov %0, r6" : "=r"(svc_number));

  switch (svc_number)
  {
  case EXEC_PSP_TASK:
    if (SYSTICK) 
    {
      init_systick(200);
    }

    Tcb_t* tcb_of_pid0 = ((Tcb_t*)currently_running->data);
    __asm volatile ("mov r0, %[sp]" :: [sp] "r" (tcb_of_pid0->sp));
    __asm volatile ("ldmia.w  r0!, {r4-r11}");
    __asm volatile ("msr psp, r0");
    __asm volatile ("mov r1, 0xfffffffd");
    __asm volatile ("str r1, [sp, #4]");
    ST_ENABLE;
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
      ST_DISABLE;
    }
    restore_psp();
    return;
  case STE:
    if (SYSTICK)
    {
      ST_ENABLE;
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
  default:
    __builtin_unreachable();
    break;
  }
}
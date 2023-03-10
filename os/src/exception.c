#include "exception.h"
#include "lang.h"
#include "hw/cpu.h"
#include "process/task.h"
#include "memory.h"
#include "test.h"
#include "devices/uart.h"
#include "process/scheduler.h"

volatile unsigned int svc_number = 0;

void __attribute__((__noipa__)) SysTick()
{
  // disable_systick();
  
  save_psp_if_threadmode();

  if (DEBUG)
  {
    Tcb_t* tcb_of_current_task = ((Tcb_t*)currently_running->data);
    tcb_of_current_task->lifetime_info[0].lifetime.forced_interrupts++;
    process_stats->num_of_systick_interrupts++;
  }
    __asm volatile ("mrs %0, psp" : "=r"(((Tcb_t*) task_to_preserve->data)->sp));
    switch_task();
    unsigned int next = ((Tcb_t*) currently_running->data)->sp;
    if (!next)
    {
      while (1)
      {
        /* code */
      }
      
    }

  // if (next < 0x200007a8)
  // {

  // while (1)
  // {
  //   /* code */
  // }
  // }
    __asm volatile ("mov r2, %[next_sp]":: [next_sp] "r" (next));
    __asm volatile (
      "ldmfd r2!, {r4-r11}\n"
      "msr psp, r2\n"
      "mrs r1, msp\n"
      "add sp, sp, #16\n"
      "bx lr\n"
    );
    
  
}

__attribute__((used))  void uprint(volatile unsigned int* transfer_info __attribute__((unused)))
{
  SV_PRINT;
}

__attribute__((used))  void execute_priviledged(unsigned int function_address)
{
  SV_EXEC_PRIV;
}

void __attribute__((optimize("O0"))) kprint()
{
  volatile TransferInfo_t* t;
  __asm__("mov %0, r0" : "=r"(t));
  setup_transfer((char*) t->start_adress, t->length);
}

void __attribute__((optimize("O3"))) SVCall()
{
  if (SYSTICK)
    disable_systick();

  if (DEBUG)
    process_stats->num_of_svcalls++;
  
  save_psp_if_threadmode();
  __asm__("mov %0, r6" : "=r"(svc_number));

  switch (svc_number)
  {
  case EXEC_PSP_TASK:
    if (SYSTICK)
      init_systick(5);

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
    if (DEBUG)
    {
      Tcb_t* tcb_of_current_task = ((Tcb_t*)currently_running->data);
      tcb_of_current_task->lifetime_info[0].lifetime.voluntary_interrupts++;
    }
    *(unsigned int*) Icsr = *(unsigned int*) Icsr | 1 << PendSVSet;
    break;
  case STD:
    if (SYSTICK)
    {
      disable_systick();
      restore_psp();
    }
    return;
  case STE:
    if (SYSTICK)
    {
      enable_systick();
      restore_psp();
    }
    return;
  case EXEC_PRIV:
    unsigned int function_adress;
    __asm__("mov %0, r0" : "=r"(function_adress));
    void (*priv_fn)() = (void (*)()) (function_adress | 1);
    priv_fn();
    restore_psp();
    return;     
  default:
    __builtin_unreachable();
    break;
  }
  if (SYSTICK)
    enable_systick();
}
#include "exception.h"
#include "types.h"
#include "lang.h"
#include "hw/cpu.h"
#include "process/task.h"
#include "memory.h"
#include "test.h"
#include "devices/uart.h"
#include "process/scheduler.h"

volatile unsigned int svc_number = 0;

void __attribute__((__noipa__))  __attribute__((optimize("O0"))) SysTick()
{
  disable_systick();
  save_psp_if_threadmode();
  Tcb_t* task_block = (Tcb_t*) task_to_preserve->data;
  __asm volatile ("mrs %0, psp" : "=r"(task_block->sp));
  unsigned int lr;
  __asm volatile ("mov lr, %0" : "=r"(lr));

  if (DEBUG)
  {
    process_stats->num_of_systick_interrupts++;
    task_block->lifetime_info[0].lifetime.forced_interrupts++;
  }
  
  switch_task();
  __asm volatile ("mov r2, %[next_sp]":: [next_sp] "r" (((Tcb_t*) currently_running->data)->sp));
  __asm volatile (
    "ldmfd r2!, {r4-r11}\n"
    "msr psp, r2\n"
  );
  __asm volatile ("mov r1, 0xfffffffd");
  __asm volatile ("str r1, [sp, #4]");
  unsigned int fake_sp;
  __asm volatile ("mrs %0, msp" : "=r"(fake_sp));
  // if (fake_sp < 0x200057b4)
  // {
  //   while (1)
  //   {
  //     /* code */
  //   }
    
  // }
}

// function exists to preserve R0 / R1 register
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
      init_systick(30);

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
      __asm volatile (
        "mrs r2, psp\n"
        "ldmfd r2!, {r4-r11}\n"
        "msr psp, r2\n"
      );
    }
    return;
  case STE:
    if (SYSTICK)
    {
      enable_systick();
      __asm volatile (
        "mrs r2, psp\n"
        "ldmfd r2!, {r4-r11}\n"
        "msr psp, r2\n"
      );
    }
    return;
  case EXEC_PRIV:
    unsigned int function_adress;
    __asm__("mov %0, r0" : "=r"(function_adress));
    void (*priv_fn)() = (void (*)()) (function_adress | 1);
    priv_fn();
      __asm volatile (
        "mrs r2, psp\n"
        "ldmfd r2!, {r4-r11}\n"
        "msr psp, r2\n"
      );
    return;     
  default:
    __builtin_unreachable();
    break;
  }
  if (SYSTICK)
    enable_systick();

  *(unsigned int*) Icsr = *(unsigned int*) Icsr | 1 << PendSVSet;
}
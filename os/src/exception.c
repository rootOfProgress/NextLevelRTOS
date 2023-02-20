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
  // disable_systick();
  if (DEBUG)
    process_stats->num_of_systick_interrupts++;
  
  save_psp_if_threadmode();
    __asm volatile ("mrs %0, psp" : "=r"(((Tcb_t*) task_to_preserve->data)->sp));
    switch_task();
    __asm volatile ("mov r2, %[next_sp]":: [next_sp] "r" (((Tcb_t*) currently_running->data)->sp));
    __asm volatile (
      "ldmfd r2!, {r4-r11}\n"
      "msr psp, r2\n"
    );
    // __asm volatile (
      // "mrs r2, psp\n"
      // "ldmfd r2!, {r4-r11}\n"
      // "msr psp, r2\n"
    // );
  // *(unsigned int*) Icsr = *(unsigned int*) Icsr | 1 << PendSVSet;
}

// function exists to preserve R0 / R1 register
__attribute__((used))  void uprint(volatile unsigned int* transfer_info __attribute__((unused)), volatile unsigned int type __attribute__((unused)))
{
  SV_PRINT;
}

void __attribute__((optimize("O0"))) kprint()
{
  volatile TransferInfo_t* t;
  unsigned int type;
  __asm__("mov %0, r0" : "=r"(t));
  __asm__("mov %0, r1" : "=r"(type));
  setup_transfer((char*) t->start_adress, t->length, type);
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
  default:
    __builtin_unreachable();
    break;
  }
  if (SYSTICK)
    enable_systick();

  *(unsigned int*) Icsr = *(unsigned int*) Icsr | 1 << PendSVSet;
}
#include "exception.h"
#include "types.h"
#include "lang.h"
#include "hw/cpu.h"
#include "process/task.h"
#include "memory.h"
#include "test.h"
#include "devices/uart.h"
#include "process/scheduler.h"

unsigned int svc_code = 0; 
volatile unsigned int svc_number = 0;

void __attribute__((__noipa__))  __attribute__((optimize("O0"))) SysTick()
{
  __asm (
      "MRS r2, PSP\n"
      "STMDB r2!, {r4-r11}\n"
      "MSR PSP, r2\n"
  );
  *(unsigned int*) Icsr = *(unsigned int*) Icsr | 1 << PendSVSet;
}

// function exists to preserve R0 register
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
  
  __asm (
    "TST lr, #4\n"
    "ITTT NE\n"
    "MRSNE r2, PSP\n"
    "STMDBNE r2!, {r4-r11}\n"
    "MSRNE PSP, r2\n"
  );
  __asm__("mov %0, r6" : "=r"(svc_number));

  switch (svc_number)
  {
  case EXEC_PSP_TASK:
    init_systick(1000);
    unsigned int sp1 = (unsigned int) ((Tcb_t*)currently_running->data)->sp;
    __asm__ volatile ("MOV R0, %[input_i]"
      :  
      : [input_i] "r" (sp1)
        );
    __asm__("ldmia.w  r0!, {r4, r5, r6, r7, r8, r9, sl, fp}");
    __asm__("msr psp, r0");
    __asm("mov r1, 0xFFFFFFFD");
    __asm("str r1, [sp, #4]");
    break;
  case PRINT_MSG:
    kprint();
  case YIELD_TASK:
    *(unsigned int*) Icsr = *(unsigned int*) Icsr | 1 << PendSVSet;
    break;
  default:
    __builtin_unreachable();
    break;
  }
  if (SYSTICK)
    enable_systick();
}
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

#ifdef SELF_CHECK

void do_selfcheck_svc()
{
  unsigned int test_a = 1;
  unsigned int test_b = 1;
  unsigned int test_c = 1;
  unsigned int test_d = 1;
  svc(3);
  if (svc_code != 3)
    test_a = 0;
  svc(7);
  if (svc_code != 7)
    test_b = 0;
  svc(9);
  if (svc_code != 9)
    test_c = 0;
  svc(0xAB);
  if (svc_code != 0xAB)
    test_d = 0;

}
#endif

void __attribute__((__noipa__))  __attribute__((optimize("O0"))) SysTick()
{
  while (1)
  {
    /* code */
  }
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
  // disable_systick();
  __asm (
    "TST lr, #4\n"
    "ITTT NE\n"
    "MRSNE r2, PSP\n"
    "STMDBNE r2!, {r4-r11}\n"
    "MSRNE PSP, r2\n"
  ) ;
  __asm__("mov %0, r6" : "=r"(svc_number));

  switch (svc_number)
  {
  case EXEC_PSP_TASK:
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
}
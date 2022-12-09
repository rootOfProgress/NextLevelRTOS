#include "exception.h"
#include "types.h"
#include "lang.h"
#include "uart.h"
#include "hw/cpu.h"
#include "process/task.h"
#include "test.h"
#include "process/scheduler.h"

// svc_code = 0;
unsigned int svc_code = 0; 
unsigned int svc_number = 0;

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

void SVC_Handler(void)
{

}
void /* __attribute__((optimize("O2"))) */ SVCall()
{
  __asm (
    "TST lr, #4\n"
    "ITTT NE\n"
    "MRSNE r2, PSP\n"
    "STMDBNE r2!, {r4-r11}\n"
    "MSRNE PSP, r2\n"
  ) ;
  __asm__("mov %0, r6" : "=r"(svc_number));

  #ifdef SELF_CHECK
    svc_code = svc_number;
    return;
    // do_selfcheck_memory();
  #endif

  switch (svc_number)
  {
  case EXEC_PSP_TASK:
    unsigned int sp1 = (unsigned int) ((Tcb_t*)currently_running->data)->sp;
    __asm__ volatile ("MOV R0, %[input_i]"
      :  
      : [input_i] "r" (sp1)
        );
    __asm__("mov lr, 0xFFFFFFFD");
    __asm__("ldmia.w  r0!, {r4, r5, r6, r7, r8, r9, sl, fp}");
    __asm__("msr psp, r0"); // move r0 value to psp
    break;
  case YIELD_TASK:
    *(unsigned int*) Icsr = *(unsigned int*) Icsr | 1 << PendSVSet;
    break;
  default:
    break;
  }


}
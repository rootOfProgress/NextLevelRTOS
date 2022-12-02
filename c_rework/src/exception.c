#include "exception.h"
#include "types.h"
#include "lang.h"
#include "uart.h"
#include "hw/cpu.h"
#include "process/scheduler.h"

void SVC_Handler_Main( unsigned int *svc_args )
{
  unsigned int svc_number;

  /*
  * Stack contains:
  * r0, r1, r2, r3, r12, r14, the return address and xPSR
  * First argument (r0) is svc_args[0]
  */
  svc_number = ( ( char * )svc_args[ 6 ] )[ -2 ] ;
  switch( svc_number )
  {
    case 0:  /* EnablePrivilegedMode */
      // __set_CONTROL( __get_CONTROL( ) & ~CONTROL_nPRIV_Msk ) ;
      break;
    default:    /* unknown SVC */
      break;
  }
}


void SVC_Handler(void)
{

}
void SVCall(TrapType_t type, unsigned int arg)
{
    __asm(
    ".global SVC_Handler_Main\n"
    "TST lr, #4\n"
    "ITE EQ\n"
    "MRSEQ r0, MSP\n"
    "MRSNE r0, PSP\n"
    "B SVC_Handler_Main\n"
  ) ;
  // __asm__("TST lr, \#4\n");
  // __asm__("ITTEE EQ\n");
  // __asm__("moveq r2, \#0\n");
  // __asm__("nop\n");
  // // __asm__("nop\n");
  // __asm__("MRSNE r2,psp\n");
  // __asm__("stmdbne r2!, {r4-r11}");
  // // __asm__("MSRNE psp, r2");

  // unsigned int r2;
  // __asm__("mov %0, r2" : "=r"(r2));
  // if (r2 != 0)
  //   ((Tcb_t*) currently_running->data)->sp = (unsigned int*) r2;
  // // unsigned int old_sp;
  // // __asm__("mov r0, %0" : "=r"(sp1));

  

  // switch (type)
  // {
  // case RUN_THREAD_MODE:
  //   __asm__("mov lr, 0xFFFFFFFD");
  //   __asm__("ldmia.w  r1!, {r4, r5, r6, r7, r8, r9, sl, fp}");
  //   __asm__("msr psp, r1"); // move r0 value to psp
  //   __asm__("bx lr");
  //   break;
  // case YIELD_TASK:
  //   *(unsigned int*) Icsr = *(unsigned int*) Icsr | 1 << PendSVSet;
  //   break;
  // default:
  //   break;
  // }

}
#include "exception.h"
#include "types.h"
#include "lang.h"
#include "uart.h"
#include "hw/cpu.h"
#include "process/scheduler.h"


void SVCall(TrapType_t type)
{
  __asm__ volatile("TST lr, \#4\n");
  __asm__ volatile("ITTTE NE\n");
  __asm__ volatile("mrsne r2,psp\n");
  __asm__ volatile("stmdbne r2!, {r4-r11}");
  __asm__ volatile("msrne psp, r2");
  __asm__ volatile("nop");

  // __asm__("TST lr, \#4\n");
  // __asm__("ITTEE EQ\n");
  // __asm__("moveq r2, \#0\n");
  // __asm__("nop\n");
  // // __asm__("nop\n");
  // __asm__("MRSNE r2,psp\n");
  // __asm__("stmdbne r2!, {r4-r11}");
  // // __asm__("MSRNE psp, r2");

  // unsigned int r0;
  // __asm__("mov %0, r2" : "=r"(r0));
  // if (r2 != 0)
  //   ((Tcb_t*) currently_running->data)->sp = r2;

  

  switch (type /* (TrapType_t) r0 */)
  {
  case RUN_THREAD_MODE:
    __asm__("mov lr, 0xFFFFFFFD");
    __asm__("ldmia.w  r1!, {r4, r5, r6, r7, r8, r9, sl, fp}");
    __asm__("msr psp, r1"); // move r0 value to psp
    __asm__("bx lr");
    break;
  case YIELD_TASK:
    *(unsigned int*) Icsr = *(unsigned int*) Icsr | 1 << PendSVSet;
    break;
  default:
    break;
  }
  // // __asm__("add sp, #12");
  // __asm__(
  //   "TST lr, #4\n"
  //   "ITTEE EQ\n"
  //   "MRSEQ r0, MSP\n"
  //   "moveq r2,4\n"
  //   "MRSNE r0, PSP\n"
  //   "movne r2,5\n"
  //   "mov r3,r0\n"
  //   "stmdb r0!, {r4, r5, r6, r7, r8, r9, sl, fp}\n"
  //   "msr msp, r0\n"
  //   // "cmp r1,#1\n"
  //   // "ITE EQ\n"
  //   // "msreq msp, r0\n"
  //   // "msrne psp, r0\n"
  // );
  // // __asm__("push {r4 - r11}");
  //   __asm__("mov %0, r2" : "=r"(bar));
  // if (bar == 4)
  // {

  //   __asm__("mrs %0, msp" : "=r"(sp_msp));
  // }
  // else if (bar == 5)
  // {

  //   __asm__("mrs %0, psp" : "=r"(sp_psp));
  // }
  // // msp on entry
  // if (opcode == 1)
  // {
  //   for (uint32_t i = 0; i < 17; i++)
  //   {
  //     WRITE_REGISTER(0x20020000 + (i * 4), READ_REGISTER(sp_msp + i * 4));
  //   }

  //   // update relocated main sp
  //   WRITE_REGISTER(0x20020000 + (8 * 4), 0x20020000 + (9 * 4));
  //   __asm__(
  //     "ldr r0, =#0x20020000\n"
  //     "msr msp, r0");
  //   __asm__("pop {r4 - r11}");
  //   __asm__("bx lr");
  // }
  // else if (opcode == 2)
  // {
  //   volatile frame_t *task0 = (frame_t*) (0x20001000);
  //   volatile uint32_t psp = (uint32_t) & (task0->r4);
  //   __asm__("mov lr, 0xFFFFFFFD");
  //   __asm__("msr psp, %0" : "=r"(psp));
  //   __asm__("mrs r0, psp");
  //   __asm__("ldmia.w  r0!, {r4, r5, r6, r7, r8, r9, sl, fp}");
  //   __asm__("msr psp, r0");
  //   // __asm__("mov sp, %0" : "=r"(psp));
  //   // __asm__("msr msp, %0" : "=r"(psp));
  //   __asm__("bx lr");
  //   return;
  // } 
  // else if (opcode == 3) 
  // {
  //   __asm__("mov lr, 0xFFFFFFF9");
  //   __asm__(
  //     "ldr r0, =#0x20020000\n"
  //     "msr msp, r0");
  //   __asm__("pop {r4 - r11}");
  //   // __asm__("mrs r0, msp");
  //   // __asm__("ldmia.w  r0!, {r4, r5, r6, r7, r8, r9, sl, fp}");
  //   // __asm__("msr msp, r0");
  //   __asm__("bx lr");
  // }

}
#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "lang.h"

#define svc(code) asm volatile ("mov r6, %[immediate]\n" \
                                " SVC \#0"::[immediate] "I" (code) \
                                 : "r6")


#define SV_EXEC_PSP_TASK __asm volatile ("mov r6, 0\n" \
                                  "svc 0\n")

#define SV_YIELD_TASK __asm volatile ("mov r6, 2\n" \
                                  "svc 0\n")

#define SV_STD __asm volatile ("mov r6, 3\n" \
                                  "svc 0\n")

#define SV_STE __asm volatile ("mov r6, 4\n" \
                                  "svc 0\n")

#define SV_SLEEP __asm volatile ("mov r6, 5\n" \
                                  "mov r9, r0\n"\
                                  "svc 0\n")
                                  
#define SV_EXEC_PRIV __asm volatile ("mov r6, 6\n" \
                              "mov r9, r0\n"\
                              "svc 0\n")

#define SV_WAKEUP_PID __asm volatile ("mov r6, 7\n" \
                                  "mov r9, r0\n"\
                                  "svc 0\n")

#define SV_SET_EXT_IO_CALLBACK __asm volatile ("mov r6, 8\n" \
                                  "mov r9, r0\n"\
                                  "svc 0\n")

#define SV_GET_IO_BUFFER __asm volatile ("mov r6, 9\n" \
                                  "mov r9, r0\n"\
                                  "svc 0\n")

#define SV_PRINT __asm volatile ("mov r6, 1\n" \
                                  "mov r9, r0\n"\
                                  "svc 0\n")


#define ST_DISABLE   __asm volatile(\
      "mov.w	r2, #3758153728\n"\
      "ldr	r3, [r2, #16]\n"\
      "bic.w	r3, r3, #1\n"\
      "str	r3, [r2, #16]\n"\
  );

#define ST_ENABLE   __asm volatile(\
      "mov.w	r2, #3758153728\n"\
      "ldr	r3, [r2, #16]\n"\
      "orr.w	r3, r3, #1\n"\
      "str	r3, [r2, #16]\n"\
  );

__attribute__((used)) void uprint(volatile unsigned int*);
__attribute__((used)) void execute_priviledged(unsigned int);

static inline __attribute__((always_inline)) void save_psp_if_threadmode(void)
{
  __asm (
    "TST lr, #4\n"
    "ITTT NE\n"
    "MRSNE r2, PSP\n"
    "STMDBNE r2!, {r4-r11}\n"
    "MSRNE PSP, r2\n"
  );
}

typedef enum {
    EXEC_PSP_TASK = 0,
    PRINT_MSG,
    YIELD_TASK,
    STD,
    STE,
    RESET,
    EXEC_PRIV,
    WAKEUP_PID,
    SET_EXT_IO_CALLBACK = 8,
    GET_IO_BUFFER,
} TrapType_t;

typedef struct UsageFaultStatus {
  unsigned int undefinstr : 1, invstate : 1, invpc : 1,  nocp : 1,  : 4, unaligned : 1, divbyzero : 1, : 6, : 16;
} UsageFaultStatus_t;

void do_selfcheck_svc();

#endif
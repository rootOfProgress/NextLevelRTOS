#ifndef EXCEPTION_H
#define EXCEPTION_H
#include <stdint.h>
#include "lang.h"

#ifdef SELF_CHECK
    // do_selfcheck_memory();
#endif

#define svc(code) asm volatile ("mov r6, %[immediate]\n" \
                                " SVC \#0"::[immediate] "I" (code) \
                                 : "r6")

typedef enum {
    RET_PSP_THREAD_NOFP = 0xFFFFFFFD,
    RET_MSP_THREAD_NOFP = 0xFFFFFFF9,
} ExcReturn_t;

typedef enum {
    EXEC_PSP_TASK = 0,
    YIELD_TASK,
    RESET
} TrapType_t;

void do_selfcheck_svc();

#endif
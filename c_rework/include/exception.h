#ifndef EXCEPTION_H
#define EXCEPTION_H
#include <stdint.h>
#include "lang.h"

// extern unsigned int svc_code = 0;
#ifdef SELF_CHECK
    // do_selfcheck_memory();
#endif

// extern volatile uint32_t sp_msp;
// extern volatile uint32_t sp_psp;
// extern volatile uint32_t opcode;
// extern volatile void* arg;

#define svc(code) asm volatile ("mov r6, %[immediate]\n" \
                                " SVC \#0"::[immediate] "I" (code) \
                                 : "r6")

typedef enum {
    RUN_THREAD_MODE = 0,
    YIELD_TASK,
    DEVICE_REQUEST,
    RESET
} TrapType_t;

void do_selfcheck_svc();

#endif
#ifndef EXCEPTION_SVC_H
#define EXCEPTION_SVC_H

#include "lang.h"

static inline __attribute__((always_inline)) NO_OPT void svcall( unsigned int svc_number)
{
__asm volatile ("mov r6, %[input]"
    : // no C variable outputs
    : [input] "r" (svc_number)
    : "r6"   // tell the compiler r9 is modified
      );
__asm volatile ("svc 0\n");
}

static inline __attribute__((always_inline)) NO_OPT void svcall_param(UNUSED unsigned int svc_number, UNUSED unsigned int parameter)
{
__asm volatile ("mov r6, %[input]"
    : // no C variable outputs
    : [input] "r" (svc_number)
    : "r9"   // tell the compiler r9 is modified
      );
__asm volatile ("mov r9, %[input]"
    : // no C variable outputs
    : [input] "r" (parameter)
    : "r9"   // tell the compiler r9 is modified
      );
__asm volatile ("svc 0\n");

}

#endif
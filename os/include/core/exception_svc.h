#ifndef EXCEPTION_SVC_H
#define EXCEPTION_SVC_H

#include "lang.h"

typedef enum
{
  execPspTask = 0,
  printMsg,
  yieldTask,
  std,
  ste,
  reset,
  execPriv,
  wakeupPid,
  setExtIoCallback = 8,
  getIoBuffer,
  wakeupIoHandler,
  disableIrReception,
  enableIrReception,
  getLifetime
} TrapType_t;

static inline __attribute__((always_inline)) NO_OPT void svcall(unsigned int svc_number)
{
  __asm volatile ("mov r6, %[input]"
                  : // no C variable outputs
                  : [input] "r" (svc_number)
                  : "r6"   // tell the compiler r9 is modified
                 );
  __asm volatile ("svc 0\n");
}

static inline __attribute__((always_inline)) NO_OPT void svcall_param(unsigned int svc_number, unsigned int parameter)
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
#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "lang.h"


#define SV_SET_EXT_IO_CALLBACK __asm volatile ("mov r6, 8\n" \
                                  "mov r9, r0\n"\
                                  "svc 0\n")

#define SV_GET_IO_BUFFER __asm volatile ("mov r6, 9\n" \
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

typedef enum {
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
} TrapType_t;

typedef struct UsageFaultStatus {
  unsigned int undefinstr : 1, invstate : 1, invpc : 1,  nocp : 1,  : 4, unaligned : 1, divbyzero : 1, : 6, : 16;
} UsageFaultStatus_t;


#endif
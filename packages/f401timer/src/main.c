#define SV_YIELD_TASK __asm volatile ("mov r6, 2\n" \
                                  "svc 0\n")

#define SV_PRINT __asm volatile ("mov r6, 1\n" \
                                  "svc 0\n")

#include "os_api.h"
#include "tim2_5.h"

int __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{
    return 0;
}
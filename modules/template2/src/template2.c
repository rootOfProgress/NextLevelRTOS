#define SV_YIELD_TASK __asm volatile ("mov r6, 2\n" \
                                  "svc 0\n")

#define SV_PRINT __asm volatile ("mov r6, 1\n" \
                                  "svc 0\n")

#include "dma.h"

void __attribute__((__noipa__))  __attribute__((optimize("O0"))) template2(void)
{
    while (1)
    {
        /* code */
        SV_YIELD_TASK;
    }
    
}

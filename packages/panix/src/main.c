#include "os_api.h"

int __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{
   svcall_param(setDebugPanic, 1);
}

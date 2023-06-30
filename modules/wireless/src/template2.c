#define SV_YIELD_TASK __asm volatile ("mov r6, 2\n" \
                                  "svc 0\n")

#include "os_lib.h"
#include "nrf24l01.h"

int fibbonacci(int n) {
   if(n == 0){
      return 0;
   } else if(n == 1) {
      return 1;
   } else {
      return (fibbonacci(n-1) + fibbonacci(n-2));
   }
}

void __attribute__((__noipa__))  __attribute__((optimize("O0"))) template2(void)
{
    while (1)
    {
      Nrf24l01Registers_t t;
      t.config = 3;
        fibbonacci(10);
        task_sleep(1000);
        SV_YIELD_TASK;
    }
    
}

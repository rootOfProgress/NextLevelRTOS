#define SV_YIELD_TASK __asm volatile ("mov r6, 2\n" \
                                  "svc 0\n")

#define SV_PRINT __asm volatile ("mov r6, 1\n" \
                                  "svc 0\n")

#include "os_api.h"
#include "tim2_5.h"
#include "am2302.h"
int __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{
    am2302_do_measurement();
    // // char ccr[4] = {0,0,0,0};

    // // examples: 
    // // tick: 16 MHZ, using PSC of 1 (+1) -> 2 -> 8MHZ effectivly.
    // // tick: 16 MHZ, using PSC of 1 (+1) -> 2 -> 8MHZ effectivly.
    // const int timer_number = 3;


    // timer_init(timer_number, 1000, (char[4]) {0,0,0,0}, 1000);
    // BREAK;
    // timer_start(timer_number);

    // // 2 sek: 0xF42400 * 1/8MHZ = 1 sek
    // while (read_counter(timer_number) < 0x3E8)
    // {
    
    // }
    // // flush_counter(2);
    // timer_stop(timer_number);
    // BREAK;

    // }

    return 0;
}
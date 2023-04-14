#define SV_YIELD_TASK __asm volatile ("mov r6, 2\n" \
                                  "svc 0\n")

#define SV_PRINT __asm volatile ("mov r6, 1\n" \
                                  "svc 0\n")

#define SV_STD __asm volatile ("mov r6, 3\n" \
                                  "svc 0\n")

#define SV_STE __asm volatile ("mov r6, 4\n" \
                                  "svc 0\n")

#include "os_api.h"
#include "tim2_5.h"
#include "am2302.h"

int __attribute((section(".isr1"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) isr1(void)
{
    while (1)
    {
        BREAK;
    }
}


int __attribute((section(".isr2"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) isr2(void)
{
    // while (1)
    // {
        unsigned tim_sr = get_sr(3);
        // if (tim_sr & 1)
        // {
        //     BREAK;
        //     set_sr(3, tim_sr & ~1);
        //     BREAK;

        // }
        if (tim_sr & (1 << 1))
        {
            timer_stop(3);

            // acknowledge interrupt reception
            set_sr(3, tim_sr & ~(1 << 1));
            BREAK;
        }
        // return;
    // }
}

int __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{
    SV_STD;
    // am2302_init_peripherials(0, 'A');
    // am2302_do_measurement();
    // // char ccr[4] = {0,0,0,0};

    // // examples: 
    // // tick: 16 MHZ, using PSC of 1 (+1) -> 2 -> 8MHZ effectivly.
    // // tick: 16 MHZ, using PSC of 1 (+1) -> 2 -> 8MHZ effectivly.
    const int timer_number = 3;

    // timer_init(timer_number, 1000, (unsigned int[4]) {0x3E8,0x3E8,0x3E8,0x3E8}, 1000);
    // timer_init(timer_number, 1000, (unsigned int[4]) {0,0,0,0}, 1000);
    // enable_ccx_ir(timerNumber, 1);
    set_ccr(timer_number, 2000, 1);
    // generate_ue(timer_number);

    // enable_ccx_ir(timerNumber, 1);
    // enable_ccx_ir(timerNumber, 2);
    // enable_ccx_ir(timerNumber, 3);
    // SV_STD;
    BREAK;
    timer_start(timer_number);

    // // // 2 sek: 0xF42400 * 1/8MHZ = 1 sek
    // while (read_counter(timer_number) < 0x3E8)
    // {
    
    // }
    // timer_stop(timer_number);
    // // flush_counter(2);
    // BREAK;
    // SV_STE;

    SV_STE;
    return 0;
}
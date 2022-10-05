#include "../include/exception.h"
#include "../include/types.h"
#include "../include/lang.h"
#include "../include/device.h"
#include "../include/uart.h"

volatile uint32_t sp;
volatile uint32_t opcode;
volatile void* arg;

// R0, R1
void trap()
{
    __asm("svc 0");
}

void SVCall()
{
    // __asm__("add sp, #12");
    __asm__("push {r4 - r11}");

    // msp on entry
    __asm__("mrs %0, msp" : "=r"(sp));
    if (opcode == 1) 
    {
        for (uint32_t i = 0; i < 17; i++)
        {
            WRITE_REGISTER(0x20020000 + (i * 4), READ_REGISTER(sp + i * 4));
        }

        // update relocated main sp
        WRITE_REGISTER(0x20020000 + (8 * 4), 0x20020000 + (9 * 4));
        __asm__(
            "ldr r0, =#0x20020000\n"
            "msr msp, r0");
            __asm__("pop {r4 - r11}");
            __asm__("bx lr");
    } else if (opcode == 2)
    {
        volatile frame_t *task0 = (frame_t*) (0x20001000);
        volatile uint32_t psp = (uint32_t) &(task0->r4);
        __asm__("mov lr, 0xFFFFFFFD");
        __asm__("msr psp, %0" : "=r"(psp));
        __asm__("mrs r0, psp");
        __asm__("ldmia.w  r0!, {r4, r5, r6, r7, r8, r9, sl, fp}");
        __asm__("msr psp, r0");
        // __asm__("mov sp, %0" : "=r"(psp));
        // __asm__("msr msp, %0" : "=r"(psp));
        __asm__("bx lr");
        return;
    } 
    
}
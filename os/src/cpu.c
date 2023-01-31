#include "hw/cpu.h"
#include "lang.h"

void init_systick(unsigned int period)
{
    unsigned int cycles_until_zero = period * 8000;

    SystickRegisters_t* stk = (SystickRegisters_t*) STK;

    WRITE_REGISTER(&stk->load, READ_REGISTER(&stk->load) & ~0x00FFFFFF);
    WRITE_REGISTER(&stk->load, cycles_until_zero);
   
    WRITE_REGISTER(&stk->val, READ_REGISTER(&stk->load) & ~0x00FFFFFF);
    WRITE_REGISTER(&stk->ctrl, READ_REGISTER(&stk->ctrl) & ~0b111);
    WRITE_REGISTER(&stk->ctrl, READ_REGISTER(&stk->ctrl) | 0b110);
}

void sleep(void)
{
    WRITE_REGISTER(SCR, READ_REGISTER(SCR) | 1 << 1);
}

void  __attribute__((optimize("O0"))) soft_reset(void)
{
    __asm("dsb");
    // __asm("ldr r1,=0x05FA0004");
    // __asm("str r1, [r0, #0]"); 
    WRITE_REGISTER(AIRCR, 0x05FA0001);
    __asm("dsb");
    while (1)
    {
        /* code */
    }
    
}
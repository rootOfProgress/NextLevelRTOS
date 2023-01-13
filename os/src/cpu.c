#include "hw/cpu.h"
#include "lang.h"

void init_systick(unsigned int period)
{
    unsigned int cycles_until_zero = period * 8000;

    SystickRegisters_t* stk = (SystickRegisters_t*) STK;

    WRITE_REGISTER(stk->load, READ_REGISTER(stk->load) & ~0x00FFFFFF);
    WRITE_REGISTER(stk->load, cycles_until_zero);
   
    WRITE_REGISTER(stk->val, READ_REGISTER(stk->load) & ~0x00FFFFFF);
    WRITE_REGISTER(stk->ctrl, READ_REGISTER(stk->ctrl) & ~0b111);
    WRITE_REGISTER(stk->ctrl, READ_REGISTER(stk->ctrl) | 0b110);
}

void soft_reset(void)
{
    WRITE_REGISTER(AIRCR, READ_REGISTER(AIRCR) | 1 << 2);
}
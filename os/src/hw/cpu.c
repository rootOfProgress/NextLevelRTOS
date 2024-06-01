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

unsigned int calculate_nvic_target_register(unsigned int nvic_number, NvicType_t nvic_type)
{
    unsigned int nvic_register_block = nvic_number / 32;
    unsigned int nvic_register_base;
    switch (nvic_type)
    {
    case NVIC_ENABLE:
        nvic_register_base = CPU_NVIC_ISER_BASE;
        break;
    case NVIC_CLEAR:
        nvic_register_base = CPU_NVIC_ICER_BASE;
        break;
    case NVIC_SET_PENDING:
        nvic_register_base = CPU_NVIC_ISPR_BASE;
        break;
    case NVIC_CLEAR_PENDING:
        nvic_register_base = CPU_NVIC_ICPR_BASE;
        break;
    case NVIC_IS_ACTIVE:
        nvic_register_base = CPU_NVIC_IABR_BASE;
        break;
    default:
        nvic_register_base = CPU_NVIC_ISER_BASE;
        break;
    }
    return nvic_register_base + (0x04 * nvic_register_block);
}

void NO_OPT enable_nvic_interrupt(unsigned nvic_number)
{
    unsigned int* nvic_enable = (unsigned int*) calculate_nvic_target_register(nvic_number, NVIC_ENABLE); 
    WRITE_REGISTER(nvic_enable, READ_REGISTER(nvic_enable) | (1 << (nvic_number % 32)));
}

void clear_nvic_interrupt(unsigned nvic_number)
{
    unsigned int* nvic_clear = (unsigned int*) calculate_nvic_target_register(nvic_number, NVIC_CLEAR_PENDING); 
    WRITE_REGISTER(nvic_clear, READ_REGISTER(nvic_clear) | (1 << (nvic_number % 32)));
}

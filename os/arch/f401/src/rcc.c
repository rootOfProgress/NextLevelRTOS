#include "rcc.h"
#include "lang.h"

unsigned int RCC_BASE_ADRESS = 0x40023800;

void activate_crc_clock(void)
{
    RccRegisterMap_t* rcc_regs = (RccRegisterMap_t*) RCC_BASE_ADRESS;

    WRITE_REGISTER(&rcc_regs->ahb1enr, READ_REGISTER(&rcc_regs->ahb1enr) | (1 << 12));    
}
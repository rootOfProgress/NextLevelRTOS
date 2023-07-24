#include "syscfg.h"
#include "rcc.h"
#include "lang.h"

void syscfg_enable_clock(void)
{
    RccRegisterMap_t* rcc_regs = (RccRegisterMap_t*) RccBaseAdress;
    WRITE_REGISTER(&rcc_regs->apb2enr, READ_REGISTER(&rcc_regs->apb2enr) | (1 << 14));


}
void syscfg_exti_config_0_3(unsigned int pin, unsigned int exti_number)
{
    unsigned int shift = exti_number * 4;
    SysCfgRegisterMap_t* syscfg_regs = (SysCfgRegisterMap_t*) SysCfgBaseAdress;

    WRITE_REGISTER(&syscfg_regs->exticr1, READ_REGISTER(&syscfg_regs->exticr1) | (pin << shift));


}
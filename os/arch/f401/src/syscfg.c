#include "syscfg.h"
#include "rcc.h"
#include "lang.h"

void syscfg_enable_clock(void)
{
    RccRegisterMap_t* rcc_regs = (RccRegisterMap_t*) RccBaseAdress;
    WRITE_REGISTER(&rcc_regs->apb2enr, READ_REGISTER(&rcc_regs->apb2enr) | (1 << 14));


}

int syscfg_exti_config_0_3(GpioObject_t* gpio)
{
    // shift means: bitfield EXTI0, or EXTI1, ... . It is mapped to the GPIO Pin, i.e.
    // Pin 0 is associated with EXTI0, Pin 1 with EXTI 1 , ...
    unsigned int shift = gpio->pin * 4;
    unsigned int port_map;
    SysCfgRegisterMap_t* syscfg_regs = (SysCfgRegisterMap_t*) SysCfgBaseAdress;

    switch (gpio->port)
    {
    case 'A':
        port_map = PAx;
        break;
    case 'B':
        port_map = PBx;
        break;
    case 'C':
        port_map = PCx;
        break;
    case 'D':
        port_map = PDx;
        break;
    case 'E':
        port_map = PEx;
        break;
    default:
        return -1;
    }

    WRITE_REGISTER(&syscfg_regs->exticr1, READ_REGISTER(&syscfg_regs->exticr1) | (port_map << shift));
    return 1;
}
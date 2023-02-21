#include "i2c.h"
#include "rcc.h"
#include "lang.h"

void init_i2c(void)
{
    RccRegisterMap_t* rcc_regs = (RccRegisterMap_t*) RCC_BASE;
    WRITE_REGISTER(&rcc_regs->apb1enr, READ_REGISTER(&rcc_regs->apb1enr) | (1 << I2C1EN));

    i2cRegisterMap_t* i2c_regs = (i2cRegisterMap_t*) I2C1_BASE;
    i2c_regs->i2c_cr1.cr1.swrst = 1; 
    i2c_regs->i2c_cr1.cr1.swrst = 0; 

    i2c_regs->i2c_cr1.cr1.pe = 1; 
}

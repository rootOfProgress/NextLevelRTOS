#include "i2c.h"
#include "rcc.h"
#include "lang.h"

i2cRegisterMap_t* i2c_regs;

char i2c_read(void)
{
    return i2c_regs->i2c_dr;
}

void i2c_start(void)
{
    i2c_regs->i2c_cr1.cr1.start = 1;
}

void i2c_stop(void)
{
    i2c_regs->i2c_cr1.cr1.stop = 1;
}

void i2c_init(void)
{
    RccRegisterMap_t* rcc_regs = (RccRegisterMap_t*) RCC_BASE;
    WRITE_REGISTER(&rcc_regs->apb1enr, READ_REGISTER(&rcc_regs->apb1enr) | (1 << I2C1EN));

    i2c_regs = (i2cRegisterMap_t*) I2C1_BASE;
    i2c_regs->i2c_cr1.cr1.swrst = 1; 
    i2c_regs->i2c_cr1.cr1.swrst = 0; 

    i2c_regs->i2c_cr1.cr1.pe = 1; 
}

#include "i2c.h"

void init_i2c(void)
{
   i2cRegisterMap_t* i2c_regs = (i2cRegisterMap_t*) I2C1_BASE;
   i2c_regs->i2c_cr1.cr1.pe = 1; 
}
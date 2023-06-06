#include "i2c.h"
#include "rcc.h"
#include "lang.h"

i2cRegisterMap_t* i2c_regs;

void i2c_local_send_address(char recipient)
{
    while (!i2c_regs->i2c_sr1.sr1.txe) { }
    i2c_regs->i2c_dr.dr = recipient;
    while (!i2c_regs->i2c_sr1.sr1.addr) { }

    READ_REGISTER(&i2c_regs->i2c_sr1.raw);
    READ_REGISTER(&i2c_regs->i2c_sr2.raw);

}

void i2c_write(char* payload, char num_of_bytes, char recipient)
{
    // enable tx mode, page 481 ref man
    recipient &= ~1;

    i2c_local_start();
    i2c_local_send_address(recipient);

    for (unsigned int j = 0; j < num_of_bytes; j++)
    {
        while (!i2c_regs->i2c_sr1.sr1.txe) { }
        i2c_regs->i2c_dr.dr = *(payload + j);
        while (!i2c_regs->i2c_sr1.sr1.btf) { }
    }

    i2c_local_stop();
    
}

char i2c_read_single(__attribute__((unused)) char target_register, __attribute__((unused)) char num_of_bytes, char recipient)
{
    // enable rx mode, page 481 ref man
    recipient |= 1;

    i2c_local_start();

    while (!i2c_regs->i2c_sr1.sr1.txe) { }
    i2c_regs->i2c_dr.dr = recipient;
    while (!i2c_regs->i2c_sr1.sr1.addr) { }

    i2c_regs->i2c_cr1.cr1.ack = 0;
    READ_REGISTER(&i2c_regs->i2c_sr1.raw);
    READ_REGISTER(&i2c_regs->i2c_sr2.raw);

    i2c_local_stop();
    while (!i2c_regs->i2c_sr1.sr1.rxne) { }

    return i2c_regs->i2c_dr.dr;
}

void i2c_local_start(void)
{
    i2c_regs->i2c_cr1.cr1.start = 1;
    while (!i2c_regs->i2c_sr1.sr1.sb) { }

}

void i2c_local_stop(void)
{
    i2c_regs->i2c_cr1.cr1.stop = 1;
}

void i2c_init(void)
{
    RccRegisterMap_t* rcc_regs = (RccRegisterMap_t*) RccBaseAdress;
    WRITE_REGISTER(&rcc_regs->apb1enr, READ_REGISTER(&rcc_regs->apb1enr) | (1 << I2C1EN));

    i2c_regs = (i2cRegisterMap_t*) I2C1_BASE;
    i2c_regs->i2c_cr1.cr1.swrst = 1; 
    i2c_regs->i2c_cr1.cr1.swrst = 0; 

    i2c_regs->i2c_cr1.cr1.pe = 1;
    i2c_regs->i2c_cr1.cr1.ack = 1; 

    // @todo set clock
    
}

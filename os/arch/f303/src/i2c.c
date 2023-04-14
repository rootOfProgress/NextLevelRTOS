#include "i2c.h"
#include "gpio.h"

I2C_Regs_t *i2c_regs;

void i2c_stop(void)
{
    WRITE_REGISTER(&i2c_regs->CR2, READ_REGISTER(&i2c_regs->CR2) | (1<< STOP));
}

void set_write_direction(void)
{
    WRITE_REGISTER(&i2c_regs->CR2, READ_REGISTER(&i2c_regs->CR2) & ~(1<< RD_WRN));
}

void set_read_direction(void)
{
    WRITE_REGISTER(&i2c_regs->CR2, READ_REGISTER(&i2c_regs->CR2) | (1<< RD_WRN));
}

void i2c_start(void)
{
    WRITE_REGISTER(&i2c_regs->CR2, READ_REGISTER(&i2c_regs->CR2) | (1<< START));
}

void request_read(void)
{
    WRITE_REGISTER(&i2c_regs->CR2, READ_REGISTER(&i2c_regs->CR2) | (1<< START));
}

void write(unsigned int slave_addr, char *payload, unsigned int length)
{
    WRITE_REGISTER(&i2c_regs->CR1, READ_REGISTER(&i2c_regs->CR1) & ~1);
    WRITE_REGISTER(&i2c_regs->CR1, READ_REGISTER(&i2c_regs->CR1) | 1);

    WRITE_REGISTER(&i2c_regs->CR2, READ_REGISTER(&i2c_regs->CR2) & ~(0x3FF << SADD_7_1));
    WRITE_REGISTER(&i2c_regs->CR2, READ_REGISTER(&i2c_regs->CR2) & ~(0xFF << NBYTES));
    // direction -> write
    WRITE_REGISTER(&i2c_regs->CR2, READ_REGISTER(&i2c_regs->CR2) & ~(1<< RD_WRN));
    WRITE_REGISTER(&i2c_regs->CR2, READ_REGISTER(&i2c_regs->CR2) | (slave_addr << SADD_7_1) | (length << NBYTES));
    
    // flush
    WRITE_REGISTER(&i2c_regs->ISR,(READ_REGISTER(&i2c_regs->ISR) & ~(1 << TXE)));

    unsigned int sent = 0;

    i2c_start();

    do 
    {
        if ((READ_REGISTER(&i2c_regs->ISR) & (1 << TXE)))
        {
            WRITE_REGISTER(&i2c_regs->TXDR, payload[sent]);
            sent++;
        }
    } while (!(READ_REGISTER(&i2c_regs->ISR) & (1 << TC)));
    
    i2c_stop();
}

void init_i2c()
{
    i2c_regs = (I2C_Regs_t*) I2C1_BASE;
}


unsigned int read(unsigned int target_register, unsigned int slave_addr, unsigned int length)
{
    WRITE_REGISTER(&i2c_regs->CR2, READ_REGISTER(&i2c_regs->CR2) & ~(0x3FF << SADD_7_1));
    WRITE_REGISTER(&i2c_regs->CR2, READ_REGISTER(&i2c_regs->CR2) & ~(0xFF << NBYTES));
    WRITE_REGISTER(&i2c_regs->CR2, READ_REGISTER(&i2c_regs->CR2) | (slave_addr << SADD_7_1) | (length << NBYTES));

    set_write_direction();
    char payload[1] = {(char) target_register};
    write(slave_addr, payload, 1);
    
    for (unsigned int i =0 ; i < 1000; i++){}
        
    set_read_direction();
    i2c_start();
    unsigned int result = READ_REGISTER(&i2c_regs->RXDR);
    while (!((READ_REGISTER(&i2c_regs->ISR) & (1 << RXNE)) != 0)) {}
    i2c_stop();
    return result;
}
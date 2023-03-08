#ifndef I2C_H 
#define I2C_H

typedef union 
{
    struct
    {
        // unsigned int sadd : 10,
        //                 rd_wrn : 1,
        //                 add10 : 1,
        //                 head10r : 1,
        //                 start : 1,
        //                 stop : 1,
        //                 nack : 1,
        //                 nbytes : 8,
        //                 reload : 1,
        //                 autoend: 1,
        //                 pecbyte : 1,
        //                         : 5;


        unsigned int            : 5,
                        pecbyte : 1,
                        autoend: 1,
                        reload : 1,
                        nbytes : 8,
                        nack : 1,
                        stop : 1,
                        start : 1,
                        head10r : 1,
                        add10 : 1,
                        rd_wrn : 1,
                        sadd : 10;
    } cr2_fields;
    unsigned int raw;
    
} i2c_cr2_t;


typedef struct I2C {
    unsigned int CR1;
    i2c_cr2_t CR2;
    unsigned int OAR1;
    unsigned int OAR2;
    unsigned int TIMINGR;
    unsigned int TIMEOUTR;
    unsigned int ISR;
    unsigned int ICR;
    unsigned int PECR;
    unsigned int RXDR;
    unsigned int TXDR;
} I2C_Regs_t;

I2C_Regs_t *i2c_regs;

void isr_handler_ev();
void isr_handler_err();

// static inline __attribute__((always_inline)) void set_write_direction(void)
// {
//     i2c_regs->CR2.cr2_fields.rd_wrn = 0;
// }

// static inline __attribute__((always_inline)) void set_read_direction(void)
// {
//     i2c_regs->CR2.cr2_fields.rd_wrn = 1;
// }

#endif
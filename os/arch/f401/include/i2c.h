#ifndef I2C_H
#define I2C_H

#define I2C1_BASE 0x40005400


typedef union i2c_cr1
{
    struct
    {
        unsigned int pe : 1, 
                     smbus : 1,
                           : 1,
                     smbtype : 1,
                     enarp : 1,
                     enpec : 1,
                     engc : 1,
                     nostretch : 1,
                     start : 1,
                     stop : 1,
                     ack : 1,
                     pos : 1,
                     pec : 1,
                     alert : 1,
                           : 1,
                     swrst : 1,
                            : 16;
    } cr1;
    unsigned int raw;
} i2c_cr1_t;

typedef union i2c_cr2
{
    struct
    {
        unsigned int freq : 6,
                          : 2,
                          : 24
    } cr1;
    unsigned int raw;
} i2c_cr2_t;

typedef union i2c_oar1
{
    struct oar1
    {
        // 0..15
        unsigned int add0 : 1, add : 9, :5, addmode : 1, : 16;
    };
    unsigned int raw;
} i2c_oar1_t;

typedef union i2c_sr1
{
    struct sr1
    {
        // 0..15
        unsigned int sb : 1, addr : 1, btf : 1, add10 : 1, stopf : 1, : 1, rxne : 1, txe : 1, berr : 1, arlo : 1, af : 1, ovr : 1, pecerr : 1, : 1, timeout : 1, smbalert : 1, : 16;
    };
    unsigned int raw;
} i2c_sr1_t;

typedef struct i2c_dr
{
    char dr;
    char reserverd[3];
} i2c_dr_t;


typedef struct i2cRegisterMap
{
    i2c_cr1_t i2c_cr1;
    unsigned int i2c_cr2;
    i2c_oar1_t i2c_oar1;
    unsigned int i2c_oar2;
    i2c_dr_t i2c_dr;
    i2c_sr1_t i2c_sr1;
} i2cRegisterMap_t;

void init_i2c(void);


#endif
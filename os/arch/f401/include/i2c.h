#ifndef I2C_H
#define I2C_H

typedef union i2c_cr1
{
    struct cr1
    {
        unsigned char pe : 1, smbus : 1, : 1, smbtype : 1, enarp : 1, enpec : 1, engc : 1, nostretch : 1;
    };
    short raw;
} i2c_cr1_t;

typedef union i2c_oar1
{
    struct oar1
    {
        // 0..15
        short add0 : 1, add : 9, :5, addmode : 1;
    };
    short raw;
} i2c_oar1_t;

typedef union i2c_sr1
{
    struct sr1
    {
        // 0..15
        short sb : 1, addr : 1, btf : 1, add10 : 1, stopf : 1, : 1, rxne : 1, txe : 1, berr : 1, arlo : 1, af : 1, ovr : 1, pecerr : 1, : 1, timeout : 1, smbalert : 1;
    };
    short raw;
} i2c_sr1_t;

typedef struct i2c_dr
{
    short dr;
} i2c_dr_t;


typedef struct i2cRegisterMap
{
    i2c_cr1_t cr1;
    i2c_oar1_t oar1;
} i2cRegisterMap_t;


#endif
#ifndef I2C_H
#define I2C_H

extern void i2c_init(void);
extern void i2c_write(char*, char, char);
extern char i2c_read_single(char, char);

#endif
#ifndef I2C_H 
#define I2C_H

#define READ_REGISTER(addr)     (*(volatile unsigned int *) (addr))
#define WRITE_REGISTER(addr, val) ((*(volatile unsigned int *) (addr)) = (unsigned int) (val))

#define I2C1_BASE 0x40005400
#define RCC 0x40021000

#define PE 0
#define PRESC 28
#define SCLDEL 20
#define SDADEL 16
#define SCLH 8
#define SCLL 0
#define START 13
#define STOP 14
#define ADDR 3
#define TXE 0
#define RD_WRN 10
#define NBYTES 16
#define AUTOEND 25
#define RXNE 2
#define SADD_7_1 1
#define TXIS 1
#define TC 6

typedef struct I2C {
    unsigned int CR1;
    unsigned int CR2;
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

void set_write_direction(void);
void set_read_direction(void);
void i2c_start(void);
void i2c_stop(void);
void init_i2c(void);
void write(unsigned int, char *, unsigned int);
unsigned int read(unsigned int, unsigned int, unsigned int);



#endif
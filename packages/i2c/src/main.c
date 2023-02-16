#define SV_YIELD_TASK __asm volatile ("mov r6, 2\n" \
                                  "svc 0\n")

#define SV_PRINT __asm volatile ("mov r6, 1\n" \
                                  "svc 0\n")
typedef struct transfer {
    void* start_adress;
    unsigned int length;
} TransferInfo_t;

#define READ_REGISTER(addr)     (*(volatile unsigned int *) (addr))
#define WRITE_REGISTER(addr, val) ((*(volatile unsigned int *) (addr)) = (unsigned int) (val))

#define BUFFERLENGTH 7
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

#define ADXL345 0x53

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

I2C_Regs_t *i2c_regs;

unsigned int get_rxdr(void)
{
    return READ_REGISTER(&i2c_regs->RXDR);
}

__attribute__((used))  void uprint(volatile unsigned int* transfer_info __attribute__((unused)), volatile unsigned int type __attribute__((unused)))
{
  SV_PRINT;
}


void num_to_char(unsigned int number, char converted[])
{   
    // unsigned int n = number;
    short cnt = BUFFERLENGTH - 1 - 2;
    while (number > 0)
    {   
        converted[cnt--] = (number % 10) + 0x30;
        number /= 10;
    }
}

void stop(void)
{
    WRITE_REGISTER(&i2c_regs->CR2, READ_REGISTER(&i2c_regs->CR2) | (1<< STOP));
}

void start(void)
{
    WRITE_REGISTER(&i2c_regs->CR2, READ_REGISTER(&i2c_regs->CR2) & ~(1<< RD_WRN));
    WRITE_REGISTER(&i2c_regs->CR2, READ_REGISTER(&i2c_regs->CR2) | (1<< START));
}

void request_read(void)
{
    WRITE_REGISTER(&i2c_regs->CR2, READ_REGISTER(&i2c_regs->CR2) | (1<< RD_WRN));
    WRITE_REGISTER(&i2c_regs->CR2, READ_REGISTER(&i2c_regs->CR2) | (1<< START));

}

void write(unsigned int data, unsigned int slave_addr)
{
    WRITE_REGISTER(&i2c_regs->CR2, READ_REGISTER(&i2c_regs->CR2) | (slave_addr << SADD_7_1 | 1 << NBYTES));

    start();

    while (!(READ_REGISTER(&i2c_regs->ISR) & (1 << TXE)) != 0) {}
    WRITE_REGISTER(&i2c_regs->TXDR, data);

    while (!(READ_REGISTER(&i2c_regs->ISR) & (1 << TC)) != 0) {}
    stop();
}

void __attribute__((__noipa__))  __attribute__((optimize("O0"))) read(unsigned int target_register, unsigned int slave_addr)
{
    WRITE_REGISTER(&i2c_regs->CR2, READ_REGISTER(&i2c_regs->CR2) | (slave_addr << SADD_7_1) | 1 << NBYTES);
    start();
    while (!(READ_REGISTER(&i2c_regs->ISR) & (1 << TXE)) != 0) {}
    WRITE_REGISTER(&i2c_regs->TXDR, target_register);
    while (!(READ_REGISTER(&i2c_regs->ISR) & (1 << TC)) != 0) {}
    stop();

    request_read();
    while (!((READ_REGISTER(&i2c_regs->ISR) & (1 << RXNE)) != 0)) {}
    stop();
}

void init()
{
}


void __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{    
    i2c_regs = (I2C_Regs_t*) I2C1_BASE;
    while (1)
    {
        read(0x00, ADXL345);
        SV_YIELD_TASK;
    }
    
    // write(0, 0x53);
}

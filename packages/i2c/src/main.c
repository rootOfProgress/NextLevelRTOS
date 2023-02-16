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

void set_write_direction(void)
{
    WRITE_REGISTER(&i2c_regs->CR2, READ_REGISTER(&i2c_regs->CR2) & ~(1<< RD_WRN));
}

void set_read_direction(void)
{
    WRITE_REGISTER(&i2c_regs->CR2, READ_REGISTER(&i2c_regs->CR2) | (1<< RD_WRN));
}

void start(void)
{
    // WRITE_REGISTER(&i2c_regs->CR2, READ_REGISTER(&i2c_regs->CR2) & ~(1<< RD_WRN));
    WRITE_REGISTER(&i2c_regs->CR2, READ_REGISTER(&i2c_regs->CR2) | (1<< START));
}

void request_read(void)
{
    // WRITE_REGISTER(&i2c_regs->CR2, READ_REGISTER(&i2c_regs->CR2) | (1<< RD_WRN));
    WRITE_REGISTER(&i2c_regs->CR2, READ_REGISTER(&i2c_regs->CR2) | (1<< START));

}

void __attribute__((__noipa__))  __attribute__((optimize("O0"))) write(unsigned int slave_addr, char *payload, unsigned int length)
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

    start();

    do 
    {
        if ((READ_REGISTER(&i2c_regs->ISR) & (1 << TXE)))
        {
            WRITE_REGISTER(&i2c_regs->TXDR, payload[sent]);
            sent++;
        }
    } while (!(READ_REGISTER(&i2c_regs->ISR) & (1 << TC)));
    
    stop();
}

unsigned int __attribute__((__noipa__))  __attribute__((optimize("O0"))) read(unsigned int target_register, unsigned int slave_addr, unsigned int length)
{
    WRITE_REGISTER(&i2c_regs->CR2, READ_REGISTER(&i2c_regs->CR2) & ~(0x3FF << SADD_7_1));
    WRITE_REGISTER(&i2c_regs->CR2, READ_REGISTER(&i2c_regs->CR2) & ~(0xFF << NBYTES));
    WRITE_REGISTER(&i2c_regs->CR2, READ_REGISTER(&i2c_regs->CR2) | (slave_addr << SADD_7_1) | (length << NBYTES));

    set_write_direction();
    char payload[1] = {(char) target_register};
    write(ADXL345, payload, 1);
        for (unsigned int i =0 ; i < 1000; i++){}
    // start();
    // while (!(READ_REGISTER(&i2c_regs->ISR) & (1 << TXE)) != 0) {}
    // WRITE_REGISTER(&i2c_regs->TXDR, target_register);
    // while (!(READ_REGISTER(&i2c_regs->ISR) & (1 << TC)) != 0) {}
    // stop();

    set_read_direction();
    start();
    unsigned int result = READ_REGISTER(&i2c_regs->RXDR);
    while (!((READ_REGISTER(&i2c_regs->ISR) & (1 << RXNE)) != 0)) {}
    stop();
    return result;
}

// void __attribute__((__noipa__))  __attribute__((optimize("O0"))) read(unsigned int target_register, unsigned int slave_addr, unsigned int)
// {
//     WRITE_REGISTER(&i2c_regs->CR2, READ_REGISTER(&i2c_regs->CR2) & ~(0x3FF << SADD_7_1));
//     WRITE_REGISTER(&i2c_regs->CR2, READ_REGISTER(&i2c_regs->CR2) & ~(0xFF << NBYTES));

//     WRITE_REGISTER(&i2c_regs->CR2, READ_REGISTER(&i2c_regs->CR2) | (slave_addr << SADD_7_1) | (1 << NBYTES));
//     start();
//     while (!(READ_REGISTER(&i2c_regs->ISR) & (1 << TXE)) != 0) {}
//     WRITE_REGISTER(&i2c_regs->TXDR, target_register);
//     while (!(READ_REGISTER(&i2c_regs->ISR) & (1 << TC)) != 0) {}
//     stop();

//     // 1,5ms, 1 cycle ca 1.5usec
//     for (unsigned int i =0 ; i < 1000; i++){}

//     request_read();
//     unsigned int result = READ_REGISTER(&i2c_regs->RXDR);
//     while (!((READ_REGISTER(&i2c_regs->ISR) & (1 << RXNE)) != 0)) {}
//     stop();
// }

signed int complement(unsigned int payload, unsigned int pos, char axis)
{
    unsigned int result = 0;

    // bit 9
    signed int sign = (payload >> (pos - 1)) & ~(0xFFFE);

    // bit 8 .. 0
    signed int value = (value) & ~(0xFE00);
    // negative
    if (sign == 1)
    {
        result = (((512 - value) + 1) * -1) / 256;
        if (axis == 'y' && result == -1)
            return 0;
    }
    else
    {
        if (axis == 'y' && ((value + 1) / 256) == 1)
            return 0;
    }
    return result;
}

typedef struct readings {
    signed int x;
    signed int y;
    signed int z;
} readings_t;

void __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{    
    i2c_regs = (I2C_Regs_t*) I2C1_BASE;
    readings_t myreadings = { .x = 0, .y = 0, .z = 0};
    // while (1)
    // {
        // read(0xD0,0x76);
        // write()
        char payload[2] = {0x2D, 8};
        write(ADXL345, &payload, 2);
        read(0x00, ADXL345, 1);

        myreadings.x |= read(0x32, ADXL345, 1);
        myreadings.x |= (read(0x33, ADXL345, 1) << 8);
        myreadings.y |= read(0x34, ADXL345, 1);
        myreadings.y |= (read(0x35, ADXL345, 1) << 8);
        myreadings.z |= read(0x36, ADXL345, 1);
        myreadings.z |= (read(0x37, ADXL345, 1) << 8);

        myreadings.x = complement(myreadings.x, 10, 'x');
        myreadings.y = complement(myreadings.y, 10, 'y');
        myreadings.z = complement(myreadings.z, 10, 'z');
        // SV_YIELD_TASK;
        
        
    // }
    
    // write(0, 0x53);
}

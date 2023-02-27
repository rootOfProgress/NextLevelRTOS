#include "fifo.h"
#include "adxl345_common.h"

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


readings_t *position_readings;

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

signed int complement(unsigned int payload, unsigned int pos, char axis)
{
    // bit 9, aka 10. bit
    unsigned int sign = (payload >> (pos - 1)) & 1;

    // bit 8 .. 0
    signed int value = payload & 0x1FF;
    // negative
    if (sign)
        return (((512 - value) + 1) * -1) / 256;

    return (value + 1) / 256;
}

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

unsigned char* read_multiple(unsigned int target_register, unsigned int slave_addr, unsigned int length)
{
    unsigned char inbuffer[length];
    unsigned char pos = 0;
    WRITE_REGISTER(&i2c_regs->CR2, READ_REGISTER(&i2c_regs->CR2) & ~(0x3FF << SADD_7_1));
    WRITE_REGISTER(&i2c_regs->CR2, READ_REGISTER(&i2c_regs->CR2) & ~(0xFF << NBYTES));
    WRITE_REGISTER(&i2c_regs->CR2, READ_REGISTER(&i2c_regs->CR2) | (slave_addr << SADD_7_1) | (length << NBYTES));

    set_write_direction();
    char payload[1] = {(char) target_register};
    write(slave_addr, payload, 1);

    set_read_direction();

    i2c_start();

    do
    {
        while (!((READ_REGISTER(&i2c_regs->ISR) & (1 << RXNE)) != 0)) {}
        inbuffer[pos++] = (unsigned char) READ_REGISTER(&i2c_regs->RXDR);

    } while (pos != length);
    i2c_stop();

    return inbuffer;
}

unsigned char read(unsigned int target_register, unsigned int slave_addr, unsigned int length)
{
    WRITE_REGISTER(&i2c_regs->CR2, READ_REGISTER(&i2c_regs->CR2) & ~(0x3FF << SADD_7_1));
    WRITE_REGISTER(&i2c_regs->CR2, READ_REGISTER(&i2c_regs->CR2) & ~(0xFF << NBYTES));
    WRITE_REGISTER(&i2c_regs->CR2, READ_REGISTER(&i2c_regs->CR2) | (slave_addr << SADD_7_1) | (length << NBYTES));

    set_write_direction();
    char payload[1] = {(char) target_register};
    write(slave_addr, payload, 1);

    set_read_direction();
    i2c_start();

    while (!((READ_REGISTER(&i2c_regs->ISR) & (1 << RXNE)) != 0)) {}
    unsigned char result = (unsigned char) READ_REGISTER(&i2c_regs->RXDR);
    i2c_stop();
    return result;
}

void init_fifo(void)
{

}

void  __attribute__((__noipa__))  __attribute__((optimize("O0"))) read_samples(void)
{
    unsigned int sample_position = 0;
    for (unsigned int reg_addr = 0x32; reg_addr < 0x38; reg_addr++)
    {
        samples[sample_position++] = read(reg_addr, ADXL345, 1);
    }
}

void  __attribute__((__noipa__))  __attribute__((optimize("O0"))) collect_samples(void)
{
    // 16 bit
    // signed int measurements[NUM_OF_SAMPLES / 2];
    // for (unsigned int reg_low = 0, reg_high = 1, idx = 0; reg_high < NUM_OF_SAMPLES; reg_low += 2, reg_high += 2, idx++)
    // {
    //     measurements[idx] = samples[reg_low] | (samples[reg_high] << 8); 
    // }

    // position_readings->x = 0;
    // position_readings->y = 0;
    // position_readings->z = 0;
    // position_readings->x = (signed int) measurements[0];
    // position_readings->y = (signed int) measurements[1];
    // position_readings->z = (signed int) measurements[2];

    position_readings->x |= (signed int) samples[0];
    position_readings->x |= (signed int) samples[1] << 8; 
    position_readings->y |= (signed int) samples[2];
    position_readings->y |= (signed int) samples[3] << 8; 
    position_readings->z |= (signed int) samples[4];
    position_readings->z |= (signed int) samples[5] << 8;    
}




void read_xyz(void)
{
    position_readings->x = 0;
    position_readings->y = 0;
    position_readings->z = 0;

    position_readings->x |= (signed int) read(0x32, ADXL345, 1);
    position_readings->x |= (signed int) (read(0x33, ADXL345, 1) << 8);
    position_readings->y |= (signed int) read(0x34, ADXL345, 1);
    position_readings->y |= (signed int) (read(0x35, ADXL345, 1) << 8);
    position_readings->z |= (signed int) read(0x36, ADXL345, 1);
    position_readings->z |= (signed int) (read(0x37, ADXL345, 1) << 8);    
}

void enable_interrupts(unsigned int irq_mask)
{
    char payload[2] = {INT_ENABLE, irq_mask};
    write(ADXL345, payload, 2);    
}

void remap_interrupts(unsigned int irq_mask)
{
    char payload[2] = {INT_MAP, irq_mask};
    write(ADXL345, payload, 2);    
}

void read_interrupt_src(void)
{
    read(INT_SOURCE, ADXL345, 1);
}

void power_on(void)
{
    char payload[2] = {POWER_CTL, 1 << 3};
    write(ADXL345, payload, 2);       
}

void write_fifo_ctl(char irq_mask)
{
    char payload[2] = {FIFO_CTL, irq_mask};
    write(ADXL345, payload, 2); 
}

void set_range(char irq_mask)
{
    char payload[2] = {DATA_FORMAT, irq_mask};
    write(ADXL345, payload, 2); 
}

void init_adxl345(void)
{
    power_on();
    remap_interrupts(1 << 7 | 1);
    enable_interrupts(IR_WATERMARK);

    fifoCtl_t fifo_ctl;
    fifo_ctl.ctl.fifo_mode = BYPASS;
    fifo_ctl.ctl.trigger = 1;
    fifo_ctl.ctl.samples = 31;
    write_fifo_ctl(fifo_ctl.raw);

    // set_range(RANGE_16G);
}


void __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{    
    i2c_regs = (I2C_Regs_t*) I2C1_BASE;
    position_readings = (readings_t*) 0x2000004c;

    init_adxl345();

// 
    // read_interrupt_src();
    // enable_interrupts(0);

    // write_fifo_ctl(0x3F);
    // // D3 -measure
    // enable_interrupts()

    // // D3 -measure
    // char payload[2] = {0x2D, 8};
    // write(ADXL345, payload, 2);

    // // D1|D0 +- 16g
    // char payload1[2] = {0x31, 3};
    // write(ADXL345, payload1, 2);

    // D7D6 - FifoMode,
    // char payload2[2] = {FIFO_CTL, (1 << 6) | 6};
    // write(ADXL345, payload2, 2);

    // for (int i = 0; i < 1500; i++)
    // {
    // }
    // read(FIFO_STATUS, ADXL345, 1);
    // read_xyz();
    while (1)
    {
        /* code */
        read_xyz();

        // stall
        for (int i = 0; i < 1500; i++)
        {
            /* code */
        }
        
        SV_YIELD_TASK;
    }
    
    // read_samples();
    // read_samples();
    // collect_samples();

    // read(FIFO_STATUS, ADXL345, 1);
    // read(INT_SOURCE, ADXL345, 1);
        // // D7D6 - FifoMode,
    // char payload3[2] = {FIFO_CTL, 0};
    // write(ADXL345, payload3, 2);

    // read_multiple(0x32, ADXL345, 3);
}

#include "drohne/rpm.h"
#include "hw/cpu.h"

#define BUFFERLENGTH 7
#define TIM2_BASE 0x40000000
#define RCC 0x40021000

#define READ_REGISTER(addr)     (*(volatile unsigned int *) (addr))
#define WRITE_REGISTER(addr, val) ((*(volatile unsigned int *) (addr)) = (unsigned int) (val))


Rpm_t rpm_results;

typedef enum transfer_types {
    GENERIC = 0,
    MEM_ADDRESS,
    MEM_STAT,
    STATISTIC,
    RPM
} TransferType_t;


void start() {
    *(unsigned int*) TIM2_BASE = *(unsigned int*) TIM2_BASE | 1;
}

void stop() {
    *(unsigned int*) TIM2_BASE = *(unsigned int*) TIM2_BASE & !1;
}

void clear_udis() {
    *(unsigned int*) TIM2_BASE = *(unsigned int*) TIM2_BASE & !(1 << 1); 
}

void flush() {
    *(unsigned int*) (TIM2_BASE | 0x24) = *(unsigned int*) (TIM2_BASE | 0x24) & !(0xFFFF); 
}

unsigned int read_timer() {
    return (*(unsigned int*) (TIM2_BASE | 0x24) & ~(1 << 31));
}

void clear_uif() {
    *(unsigned int*) (TIM2_BASE | 0x10) = *(unsigned int*) (TIM2_BASE | 0x10) & !(0xF | 0b1 << 6); 
}

void set_ug() {
    *(unsigned int*) (TIM2_BASE | 0x14) = *(unsigned int*) (TIM2_BASE | 0x14) | 1; 
}

void enable_interrupt() {
    *(unsigned int*) (TIM2_BASE | 0xC) = (*(unsigned int*) (TIM2_BASE | 0xC)) | (1 << 1); 
}

void set_ccr(unsigned int ccr) {
    *(unsigned int*) (TIM2_BASE | 0x34) = ccr;
}


void set_prescaler(unsigned int psc) {
    *(unsigned int*) (TIM2_BASE | 0x28) = psc;
}

// wrong , not needed! would affect tim3!
void reset_timer() {
    // RCC_APB1RSTR
    *(unsigned int*) 0x40021010 = *(unsigned int*) 0x40021010 | (1 << 1);
    *(unsigned int*) 0x40021010 = *(unsigned int*) 0x40021010 & !(1 << 1);
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

void __attribute__((__noipa__))  __attribute__((optimize("O0"))) init_rpm_timer()
{
    WRITE_REGISTER((RCC | 0x1C), READ_REGISTER((RCC | 0x1C)) | 1);
    // reset_timer();
    set_prescaler(1000);
    set_ug();
    clear_uif();
    clear_udis();
    set_ccr(1000);
}

void __attribute__((__noipa__))  __attribute__((optimize("O0"))) do_measurement()
{
    unsigned int rpm = 0;
    unsigned int previous = 1;
    // disable_irq();
    start();
    // tim counts with 8kHz
    // example:
    // every 125 usec one cycle
    // after 8000 (0x1f40) cycles 1 second passed
    while (read_timer() < 0x1F40)
    {
        unsigned int n = *((unsigned int*)0x48000010);
        if (((n & 1)) < previous)
            rpm++;
        previous = n & 1;
    }
    stop();
    // enable_irq();
    // 1 sec
    rpm *= 60;
    rpm_results.engine_0 = rpm;
    rpm_results.engine_1 = rpm;
    rpm_results.engine_2 = rpm;
    rpm_results.engine_3 = rpm;
    flush();
}

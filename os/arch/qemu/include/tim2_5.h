#ifndef TIM2_5_H
#define TIM2_5_H

// #include "timer_common.h"
#include "lang.h"


#define TIM2_BASE 0x40000000
#define TIM3_BASE 0x40000400
#define TIM4_BASE 0x40000800
#define TIM5_BASE 0x40000C00

#define CEN 0
#define UDIS 1
#define DIR 4
#define CEN 0

typedef struct timer25RegisterMap
{
    unsigned int cr1; // 0x00
    unsigned int cr2; // 0x00
    unsigned int smcr; // 0x00
    unsigned int dier; // 0x00
    unsigned int sr; // 0x10
    unsigned int egr; // 0x14
    unsigned int ccmr1; // 0x18
    unsigned int ccmr2; // 0x00
    unsigned int ccer; // 0x20
    unsigned int cnt; // 0x00
    unsigned int psc; // 0x28
    unsigned int arr; // 0x2c
    unsigned int reserverd; // 0x30
    unsigned int ccr1; // 0x
    unsigned int ccr2; // 0x
    unsigned int ccr3; // 0x
    unsigned int ccr4; // 0x40
    unsigned int reserverd1; // 0x44
    unsigned int dcr; // 0x
    unsigned int dmar; // 0x4c
    unsigned int tim2_or; // 0x
    unsigned int tim5_orccr4; // 0x40
    
} timer25RegisterMap_t;

static inline __attribute__((always_inline)) unsigned int get_timx_base(unsigned int tim_nr)
{
    unsigned int tim_base;
    switch (tim_nr)
    {
    case 2:
        return TIM2_BASE;
    default:
        return 0;
    }        
}

unsigned int read_counter(unsigned int);

void start(unsigned int);
void stop(unsigned int);
void init_timer(unsigned int ,unsigned int ,unsigned int ,unsigned int *);
void flush_counter(unsigned int);
void set_ccr(unsigned int,unsigned int,unsigned int);
void set_prescaler(unsigned int,unsigned int);
void clear_udis(unsigned int);

void clear_uif();

void set_ug();

void enable_interrupt();

#endif
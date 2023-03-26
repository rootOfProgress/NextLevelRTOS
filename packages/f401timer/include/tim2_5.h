#ifndef TIM2_5_H
#define TIM2_5_H

// #include "timer_common.h"
#include "lang.h"


enum { tim2Base = 0x40000000, tim3Base = 0x40000400 };

// 16 mhz
enum { ahbFrequency = 16000000 };

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
        return tim2Base;
    case 3:
        return tim3Base;
    default:
        return 0;
    }        
}

unsigned int read_counter(unsigned int);

void timer_start(unsigned int);
void timer_stop(unsigned int);

/*
 *
 * @param tim_nr number of timer 
 * @param arr auto reload value
 * @param cycle_length duration of 1 cnt value, given in microseconds
 * @param ccr* 
 */
void timer_init(unsigned int tim_nr, unsigned int arr, char *ccr, unsigned int cycle_length);

/*
 *
 * @tim_nr number of timer 
 * @param cycle_length of 1 cnt value, given in microseconds
 * 
 */
unsigned int timer_get_prescaler(unsigned int tim_nr, unsigned int cycle_length);

static inline __attribute__((always_inline)) void flush_counter(unsigned int tim_nr)
{
    unsigned int tim_base = get_timx_base(tim_nr);
    FLUSH_REGISTER(&((timer25RegisterMap_t*) tim_base)->cnt);
}

void set_ccr(unsigned int,unsigned int,unsigned int);
void set_prescaler(unsigned int,unsigned int);
void set_udis(unsigned int);

#endif
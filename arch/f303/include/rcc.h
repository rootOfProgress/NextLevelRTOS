#ifndef RCC_H
#define RCC_H

extern unsigned int RCC_BASE_ADRESS;

extern unsigned int SYSCFGEN;
extern unsigned int GPIOPAEN;
extern unsigned int GPIOPBEN;
extern unsigned int GPIOPCEN;
extern unsigned int GPIOPEEN;
extern unsigned int USART1EN;
extern unsigned int TIM2EN;
extern unsigned int TIM3EN;
extern unsigned int TIM4EN;
extern unsigned int TIM6EN;
extern unsigned int TIM15EN;
extern unsigned int I2C1EN;

typedef struct Rcc {
    unsigned int* cr;
    unsigned int* cfgr;
    unsigned int* cir;
    unsigned int* apb2rstr;
    unsigned int* apb1rstr;
    unsigned int* ahbenr;
    unsigned int* apb2enr;
    unsigned int* apb1enr;
    unsigned int* bdcr;
    unsigned int* csr;
    unsigned int* ahbrstr;
    unsigned int* cfgr2;
    unsigned int* cfgr3;
} RccRegisterMap_t;

#endif
#ifndef RCC_H
#define RCC_H

#define RCC_BASE_ADRESS 0x40021000;
#define SYSCFGEN 1;
#define GPIOPAEN 17;
#define GPIOPBEN 18;
#define GPIOPCEN 19;
#define GPIOPEEN 21;
#define USART1EN 14;
#define TIM2EN 0;
#define TIM3EN 1;
#define TIM4EN 2;
#define TIM6EN 4;
#define TIM15EN 16;
#define I2C1EN 21;

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
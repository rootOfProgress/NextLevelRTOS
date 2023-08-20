#ifndef ADC_H
#define ADC_H

#include "gpio/gpio.h"

// CR1
#define EOCIE 5
#define JEOCIE 7


// CR2
#define ADC1EN 8
#define ADON 0
#define JSWSTART 22
#define CONT 1

// SR
#define JEOC 2
#define JSTRT 3

enum {
    AdcBaseRegister = 0x40012000
};

typedef struct AdcRegisterMap {
    unsigned int sr;
    unsigned int cr1;
    unsigned int cr2;
    unsigned int smpr1;
    unsigned int smpr2;
    unsigned int jofr1;
    unsigned int jofr2;
    unsigned int jofr3;
    unsigned int jofr4;
    unsigned int htr;
    unsigned int ltr;
    unsigned int sqr1;
    unsigned int sqr2;
    unsigned int sqr3;
    unsigned int jsqr;
    unsigned int jdr1;
    unsigned int jdr2;
    unsigned int jdr3;
    unsigned int jdr4;
    unsigned int dr;
} AdcRegisterMap_t;

typedef struct AdcConfig {
    AdcRegisterMap_t* adc_registerset;
} AdcConfig_t;



void adc_init(GpioObject_t*);
void adc_turn_on(void);
void adc_start_conv_injected(void);
void adc_acknowledge_interrupt(void);
void adc_start_conv_regular(void);
unsigned int adc_read_regular_channel(void);

#endif
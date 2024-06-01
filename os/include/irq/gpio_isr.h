#ifndef GPIO_ISR_H
#define GPIO_ISR_H
#include "gpio/gpio.h"

int link_exti_src(void (*isr_callback)(), GpioObject_t *gpio_pin);
void empty_isr(void);
void init_exti_isr_handling(void);

enum {
    Exti0 = 0,
    Exti1,
    Exti2,
    Exti3,
    Exti4,
    Exti9_5,
    NumberOfSupportedExtiLines,
};


typedef struct GpioIsrConfig {
    void (*isr_handler[NumberOfSupportedExtiLines])();
    unsigned int exti_pos_vector_table;
} GpioIsrConfig;

extern GpioIsrConfig gpio_config_exti;

#endif
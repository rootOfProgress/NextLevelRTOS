#ifndef GPIO_ISR_H
#define GPIO_ISR_H
#include "gpio/gpio.h"

void link_exti_src(void (*isr_callback)(), GpioObject_t *gpio_pin);

typedef struct GpioIsrConfig {
    void (*isr_handler[2])();
    unsigned int exti_pos_vector_table;
} GpioIsrConfig;

extern GpioIsrConfig gpio_config_exti;

#endif
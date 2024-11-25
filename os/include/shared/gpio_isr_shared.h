#ifndef GPIO_ISR_SHARED_H
#define GPIO_ISR_SHARED_H

#include "../irq/gpio_isr.h"

int link_exti_src(void (*isr_callback)(), GpioObject_t *gpio_pin);

#endif
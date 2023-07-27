#include "irq/gpio_isr.h"
#include "memory.h"
#include "lang.h"
#include "hw/cpu.h"
#include "core/exception.h"

GpioIsrConfig gpio_config_exti;

void enable_cpu_interrupt_exti(void)
{
    enable_nvic_interrupt(gpio_config_exti.exti_pos_vector_table);
}

// b0 (exti0) & b1 (exti1) currently supported. general: Pin An-Hn is EXTI_n (n = 0 - 15) p295 sec 14.2.6
void link_exti_src(void (*isr_callback)(), GpioObject_t *gpio_obj)
{   
    init_gpio(gpio_obj);
    set_moder(gpio_obj, InputMode);
    switch (gpio_obj->pin)
    {
    case 0:
        gpio_config_exti.exti_pos_vector_table = 6;
        break;
    case 1:
        // set nvic, use exec priv
        gpio_config_exti.exti_pos_vector_table = 7;
        break;
    
    default:
        break;
    }
    gpio_config_exti.isr_handler[gpio_obj->pin] = isr_callback;
    execute_priviledged((unsigned int) &enable_cpu_interrupt_exti);
}

void NO_OPT INTERRUPT exti0_isr_handler()
{
    gpio_config_exti.isr_handler[0]();
}

void NO_OPT INTERRUPT exti1_isr_handler()
{
    gpio_config_exti.isr_handler[1]();
}

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

// currently supported: pin 0 - 4, every port.  general: Pin An-Hn is EXTI_n (n = 0 - 15) p203 sec 10.2
int link_exti_src(void (*isr_callback)(), GpioObject_t *gpio_obj)
{   
    // not supported
    if (gpio_obj->pin > 4)
    {
        return -1;
    }

    const int exti0_offset = 6; // page 203 refman

    init_gpio(gpio_obj);
    set_moder(gpio_obj, InputMode);
    gpio_config_exti.exti_pos_vector_table = gpio_obj->pin + exti0_offset;
    gpio_config_exti.isr_handler[gpio_obj->pin] = isr_callback;
    execute_priviledged((unsigned int) &enable_cpu_interrupt_exti);

    return 1;
}

void NO_OPT empty_isr(void)
{
    return;
}

// just to prevent hardfault on empty isr access
void init_exti_isr_handling(void)
{
    for (int i = 0; i < NumberOfSupportedExtiLines; i++)
    {
        gpio_config_exti.isr_handler[i] = empty_isr;
    }
}

void NO_OPT INTERRUPT exti0_isr_handler()
{
    gpio_config_exti.isr_handler[Exti0]();
}

void NO_OPT INTERRUPT exti1_isr_handler()
{
    gpio_config_exti.isr_handler[Exti1]();
}

void NO_OPT INTERRUPT exti2_isr_handler()
{
    gpio_config_exti.isr_handler[Exti2]();
}

void NO_OPT INTERRUPT exti3_isr_handler()
{
    gpio_config_exti.isr_handler[Exti3]();
}

void NO_OPT INTERRUPT exti4_isr_handler()
{
    gpio_config_exti.isr_handler[Exti4]();
}

#include "irq/adc_isr.h"
#include "lang.h"
#include "hw/cpu.h"
#include "core/exception.h"

AdcIsrConfig_t adc_isr_config;

void enable_cpu_interrupt_adc(void)
{
    enable_nvic_interrupt(adc_isr_config.adc_pos_vector_table);
}

void link_adc_src(void (*isr_callback)())
{   
    adc_isr_config.isr_handler = isr_callback;
    adc_isr_config.adc_pos_vector_table = 18;

    execute_priviledged((unsigned int) &enable_cpu_interrupt_adc);
}

void NO_OPT INTERRUPT adc_isr_handler()
{
    adc_isr_config.isr_handler();
}


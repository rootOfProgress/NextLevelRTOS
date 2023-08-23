#ifndef ADC_ISR_H
#define ADC_ISR_H


typedef struct AdcIsrConfig {
    void (*isr_handler)();
    unsigned int adc_pos_vector_table;
} AdcIsrConfig_t;

extern AdcIsrConfig_t adc_isr_config;

void link_adc_isr(void (*isr_callback)());

#endif
#include "adc.h"
#include "rcc.h"
#include "gpio/gpio.h"

AdcConfig_t adc_config;

void adc_enable_interrupts(void)
{
    unsigned int *cr1 = &adc_config.adc_registerset->cr1;
    WRITE_REGISTER(cr1, READ_REGISTER(cr1) | (1 << EOCIE | 1 << JEOCIE));
}

void adc_disable_interrupts(void)
{
    unsigned int *cr1 = &adc_config.adc_registerset->cr1;
    WRITE_REGISTER(cr1, READ_REGISTER(cr1) &  ~(1 << EOCIE | 1 << JEOCIE));
}

void adc_init(GpioObject_t* gpio_config)
{
    RccRegisterMap_t* rcc_regs = (RccRegisterMap_t*) RccBaseAdress;
    WRITE_REGISTER(&rcc_regs->apb2enr, READ_REGISTER(&rcc_regs->apb2enr) | (1 << ADC1EN));

    adc_config.adc_registerset = (AdcRegisterMap_t*) AdcBaseRegister;

    init_gpio(gpio_config);
    set_moder(gpio_config, ANALOG);
    set_pupdr(gpio_config, Nothing);

    // unsigned int *cr2 = &adc_config.adc_registerset->cr2;
    // unsigned int *cr1 = &adc_config.adc_registerset->cr1;
    unsigned int *smpr1 = &adc_config.adc_registerset->smpr1;
    unsigned int *smpr2 = &adc_config.adc_registerset->smpr2;
    unsigned int *sqr3 = &adc_config.adc_registerset->sqr3;

    
    // not sure if needed
    WRITE_REGISTER(smpr1, READ_REGISTER(smpr1) | (0x07FFFFFF));
    WRITE_REGISTER(smpr2, READ_REGISTER(smpr2) | (0x3FFFFFFF));

    // ch1
    WRITE_REGISTER(sqr3, READ_REGISTER(sqr3) | 1);
}


void adc_turn_on(void)
{
    unsigned int *cr2 = &adc_config.adc_registerset->cr2;

    WRITE_REGISTER(cr2, READ_REGISTER(cr2) | (1 << ADON));
}

void adc_setbit_cr2_generic(unsigned int bit_position)
{
    unsigned int *cr2 = &adc_config.adc_registerset->cr2;

    WRITE_REGISTER(cr2, READ_REGISTER(cr2) | 1 << bit_position);
}


void adc_acknowledge_interrupt(void)
{
    unsigned int *sr = &adc_config.adc_registerset->sr;
    WRITE_REGISTER(sr, READ_REGISTER(sr) & ~(1 << JEOC | 1 << JSTRT | 1 << 4 | 1 << 1));

}

void adc_start_conv_injected(void)
{

    // unsigned int *sr = &adc_config.adc_registerset->sr;
    adc_acknowledge_interrupt();

    unsigned int *cr2 = &adc_config.adc_registerset->cr2;
    if ((READ_REGISTER(cr2) & (1 << ADON)) == 0)
    {
        adc_turn_on();
    }

    WRITE_REGISTER(cr2, READ_REGISTER(cr2) | (1 << JSWSTART));
}

void adc_start_conv_regular(void)
{
    unsigned int *cr2 = &adc_config.adc_registerset->cr2;
    WRITE_REGISTER(cr2, READ_REGISTER(cr2) | (1 << 30));
}

unsigned int adc_read_regular_channel(void)
{
    return READ_REGISTER(&adc_config.adc_registerset->dr);
}

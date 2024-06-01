#include "tim2_5.h"
#include "rcc.h"
#include "gpio/gpio.h"
#define BREAK asm("bkpt") 

timerConfiguration_t timer_configurations[5];

void reset_timer(unsigned int tim_nr)
{
    SET_BIT(&((RccRegisterMap_t*) RccBaseAdress)->apb1rstr, 1 << (tim_nr - 2));    
    CLEAR_BIT(&((RccRegisterMap_t*) RccBaseAdress)->apb1rstr, 1 << (tim_nr - 2));    
}

void timer_start(unsigned int tim_nr)
{
    unsigned int tim_base = get_timx_base(tim_nr);
    SET_BIT(&((timer25RegisterMap_t*) tim_base)->cr1, 1 << CEN);
}

void timer_stop(unsigned int tim_nr)
{
    unsigned int tim_base = get_timx_base(tim_nr);
    CLEAR_BIT(&((timer25RegisterMap_t*) tim_base)->cr1, 1 << CEN);
}

void set_udis(unsigned int tim_nr) 
{
    unsigned int tim_base = get_timx_base(tim_nr);   
    WRITE_REGISTER(&((timer25RegisterMap_t*) tim_base)->cr1, READ_REGISTER(&((timer25RegisterMap_t*) tim_base)->cr1) | 1 << UDIS);    
}

void set_ccr(unsigned int tim_nr, unsigned int ccr_value, unsigned int ccr_nr)
{
    unsigned int tim_base = get_timx_base(tim_nr);
    switch (ccr_nr)
    {
    case 1:
        WRITE_REGISTER(&((timer25RegisterMap_t*) tim_base)->ccr1, ccr_value);
        break;
    case 2:
        WRITE_REGISTER(&((timer25RegisterMap_t*) tim_base)->ccr2, ccr_value);
        break;
    case 3:
        WRITE_REGISTER(&((timer25RegisterMap_t*) tim_base)->ccr3, ccr_value);
        break;
    case 4:
        WRITE_REGISTER(&((timer25RegisterMap_t*) tim_base)->ccr4, ccr_value);
        break;
    default:
        break;
    }
}

void generate_ue(unsigned int tim_nr)
{
    unsigned int tim_base = get_timx_base(tim_nr);
    WRITE_REGISTER(&((timer25RegisterMap_t*) tim_base)->egr, READ_REGISTER(&((timer25RegisterMap_t*) tim_base)->egr) | 1);
}


void set_prescaler(unsigned int tim_nr, unsigned int psc_value)
{
    unsigned int tim_base = get_timx_base(tim_nr);
    WRITE_REGISTER(&((timer25RegisterMap_t*) tim_base)->psc, psc_value);
}

void timer_set_sr(unsigned int tim_nr, unsigned int updated_value)
{
    unsigned int tim_base = get_timx_base(tim_nr);
    WRITE_REGISTER(&((timer25RegisterMap_t*) tim_base)->sr, updated_value);
}

void timer_set_arr(unsigned int tim_nr, unsigned int arr)
{
    unsigned int tim_base = get_timx_base(tim_nr);
    WRITE_REGISTER(&((timer25RegisterMap_t*) tim_base)->arr, arr);
}


unsigned int timer_get_sr(unsigned int tim_nr)
{
    unsigned int tim_base = get_timx_base(tim_nr);
    return READ_REGISTER(&((timer25RegisterMap_t*) tim_base)->sr);
}

unsigned int timer_get_prescaler(__attribute__((unused)) unsigned int tim_nr, unsigned int cycle_length)
{
    unsigned int target_frequency = ((unsigned int) (1.0f / ((float)cycle_length / 1000000.0f)));
    // unsigned int target_frequency = 1;
    // @todo: may over/underflow!!
    return (ahbFrequency / target_frequency) - 1;
}

void apply_gpio_pwm_setting(GpioObject_t *gpio)
{
    set_otyper(gpio, OpenDrain);
    set_pupdr(gpio, PullUp);
    set_moder(gpio, AlternateFunctionMode);
    into_af(gpio, 1);
}

void timer_pwm_init(unsigned int tim_nr)
{
    // timer_configurations[2].tim_nr = tim_nr;
    // timer_configurations[2].tim_registermap = (timer25RegisterMap_t*) get_timx_base(tim_nr);

    RccRegisterMap_t* rcc_regs = (RccRegisterMap_t*) RccBaseAdress;
    WRITE_REGISTER(&rcc_regs->apb1enr, READ_REGISTER(&rcc_regs->apb1enr) | 1 << (tim_nr - 2));
    
    // @todo: gpio config: use tim2, 
    // PA0 - TIM2_CH1, 
    // PA1 - TIM2_CH2, 
    // PA2 - TIM2_CH3, 
    // PA3 - TIM2_CH4,
    // AF01 
    GpioObject_t gpio_a;
    gpio_a.port = 'A';
    init_gpio(&gpio_a);

    for (unsigned int i = 0; i < 4; i++)
    {
        gpio_a.pin = i;
        apply_gpio_pwm_setting(&gpio_a);
    }
    
    // @todo: rm hardcoded 2
    unsigned int tim_base = get_timx_base(2);
    // @todo: enable channels in tim_ccmr
    // tim_ccmr is activated by default
    WRITE_REGISTER(&((timer25RegisterMap_t*) tim_base)->ccmr1, READ_REGISTER(&((timer25RegisterMap_t*) tim_base)->ccmr1) | (1 << 3) | (0b110) << 4 | (0b110) << 12);
    WRITE_REGISTER(&((timer25RegisterMap_t*) tim_base)->ccmr2, READ_REGISTER(&((timer25RegisterMap_t*) tim_base)->ccmr1) | (1 << 3) | (0b110) << 4 | (0b110) << 12);

    // @todo: enable capture compare in tim_ccer
    // CCxE
    WRITE_REGISTER(&((timer25RegisterMap_t*) tim_base)->ccer, READ_REGISTER(&((timer25RegisterMap_t*) tim_base)->ccer) | (1 << 12) | (1 << 8) | (1 << 4) | (1 << 0));
    
    // psc = 2, aka 8mhz freq
    set_prescaler(2, 1 + 1);
    
    for (unsigned int i = 1; i <= 4; i++)
    {
        set_ccr(2, 4000, i);
    }
        set_ccr(2, 1000, 1);

    //
    // example: 1kHz
    // 8000 * (1/8Mhz) = 1000Hz (aka. 3ms period)
    //
    //
    timer_set_arr(2, 8000);

    set_udis(~2);
}


void timer_init(unsigned int tim_nr, unsigned int *ccr, unsigned int cycle_length)
{
    // enable clock
    RccRegisterMap_t* rcc_regs = (RccRegisterMap_t*) RccBaseAdress;

    // For instance, TIM2EN is assigned to Bit 0, indicating Timer number 2 corresponds to Bit 0.
    WRITE_REGISTER(&rcc_regs->apb1enr, READ_REGISTER(&rcc_regs->apb1enr) | 1 << (tim_nr - 2));
    reset_timer(tim_nr);
    set_prescaler(tim_nr, timer_get_prescaler(tim_nr, cycle_length));
    generate_ue(tim_nr);
    set_udis(tim_nr);
    
    for (unsigned int i = 1; i <= 4; i++)
    {
        set_ccr(tim_nr, ccr[i], i);
    }

    timer_flush_counter(tim_nr);

    timer_configurations[tim_nr].tim_nr = tim_nr;
    timer_configurations[tim_nr].tim_registermap = (timer25RegisterMap_t*) get_timx_base(tim_nr);
}

void enable_ccx_ir(unsigned int tim_nr, unsigned int ccr_nr)
{
    unsigned int tim_base = get_timx_base(tim_nr);
    WRITE_REGISTER(&((timer25RegisterMap_t*) tim_base)->dier, READ_REGISTER(&((timer25RegisterMap_t*) tim_base)->dier) | (1 << ccr_nr));
}
#include "tim2_5.h"
#include "rcc.h"

void reset_timer(unsigned int tim_nr)
{
    // unsigned int tim_base = get_timx_base(tim_nr);
    RccRegisterMap_t* rcc_regs = (RccRegisterMap_t*) BASE_ADRESS;
    SET_BIT(&((RccRegisterMap_t*) BASE_ADRESS)->apb1rstr, 1 << tim_nr);    
    CLEAR_BIT(&((RccRegisterMap_t*) BASE_ADRESS)->apb1rstr, 1 << tim_nr);    
}


void start(unsigned int tim_nr)
{
    unsigned int tim_base = get_timx_base(tim_nr);
    SET_BIT(&((timer25RegisterMap_t*) tim_base)->cr1, 1 << CEN);
}

void stop(unsigned int tim_nr)
{
    unsigned int tim_base = get_timx_base(tim_nr);
    CLEAR_BIT(&((timer25RegisterMap_t*) tim_base)->cr1, 1 << CEN);
}

void clear_udis(unsigned int tim_nr) 
{
    unsigned int tim_base = get_timx_base(tim_nr);
    CLEAR_BIT(&((timer25RegisterMap_t*) tim_base)->cr1, 1 << UDIS);    
}

unsigned int read_counter(unsigned int tim_nr)
{
    unsigned int tim_base = get_timx_base(tim_nr);
    return READ_REGISTER(&((timer25RegisterMap_t*) tim_base)->cnt);
}

void flush_counter(unsigned int tim_nr)
{
    unsigned int tim_base = get_timx_base(tim_nr);
    FLUSH_REGISTER(&((timer25RegisterMap_t*) tim_base)->cnt);
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

void set_prescaler(unsigned int tim_nr, unsigned int psc_value)
{
    unsigned int tim_base = get_timx_base(tim_nr);
    WRITE_REGISTER(&((timer25RegisterMap_t*) tim_base)->psc, psc_value);
}


void init_timer(unsigned int tim_nr,unsigned int psc,unsigned int arr,unsigned int *ccr)
{
    // enable clock
    RccRegisterMap_t* rcc_regs = (RccRegisterMap_t*) BASE_ADRESS;
    SET_BIT(&((RccRegisterMap_t*) BASE_ADRESS)->apb1rstr, 1 << tim_nr);

    reset_timer(tim_nr);
    set_prescaler(tim_nr, psc);
    clear_udis(tim_nr);
    for (unsigned int i = 0; i < 4; i++)
    {
        set_ccr(tim_nr, ccr[i], i);
    }
    
}
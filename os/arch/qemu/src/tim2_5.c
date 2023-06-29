#include "tim2_5.h"
#include "rcc.h"
#define BREAK asm("bkpt") 

void reset_timer(__attribute__((unused)) unsigned int tim_nr)
{   
}

void timer_start(__attribute__((unused)) unsigned int tim_nr)
{
}

void timer_stop(__attribute__((unused)) unsigned int tim_nr)
{
}

void set_udis(__attribute__((unused)) unsigned int tim_nr) 
{
}

void set_ccr(__attribute__((unused)) unsigned int tim_nr, __attribute__((unused)) unsigned int ccr_value, __attribute__((unused)) unsigned int ccr_nr)
{
}

void generate_ue(__attribute__((unused)) unsigned int tim_nr)
{
}

void timer_set_sr(__attribute__((unused)) unsigned int tim_nr,__attribute__((unused))  unsigned int updated_value)
{
}


void set_prescaler(__attribute__((unused)) unsigned int tim_nr, __attribute__((unused))  unsigned int psc_value)
{
}

unsigned int timer_get_sr(__attribute__((unused)) unsigned int tim_nr)
{
    return 0;
}

unsigned int timer_get_prescaler(__attribute__((unused)) unsigned int tim_nr, __attribute__((unused)) unsigned int cycle_length)
{
    return 0;
}


void timer_init(__attribute__((unused)) unsigned int tim_nr, __attribute__((unused)) unsigned int arr, __attribute__((unused)) unsigned int *ccr, __attribute__((unused)) unsigned int cycle_length)
{
}

void enable_ccx_ir(__attribute__((unused)) unsigned int tim_nr, __attribute__((unused)) unsigned int ccr_nr)
{
}
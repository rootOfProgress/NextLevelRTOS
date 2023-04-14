#include "tim2_5.h"
#include "rcc.h"
#define BREAK asm("bkpt") 

void reset_timer(unsigned int tim_nr)
{   
}

void timer_start(unsigned int tim_nr)
{
}

void timer_stop(unsigned int tim_nr)
{
}

void set_udis(unsigned int tim_nr) 
{
}

void set_ccr(unsigned int tim_nr, unsigned int ccr_value, unsigned int ccr_nr)
{
}

void generate_ue(unsigned int tim_nr)
{
}


void set_prescaler(unsigned int tim_nr, unsigned int psc_value)
{
}

void set_sr(unsigned int tim_nr, unsigned int updated_value)
{
}

unsigned int get_sr(unsigned int tim_nr)
{
}

unsigned int timer_get_prescaler(unsigned int tim_nr, unsigned int cycle_length)
{
}


void timer_init(unsigned int tim_nr, unsigned int arr, unsigned int *ccr, unsigned int cycle_length)
{
}

void enable_ccx_ir(unsigned int tim_nr, unsigned int ccr_nr)
{
}
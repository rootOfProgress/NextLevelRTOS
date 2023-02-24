#include "gpio.h"
#include "rcc.h"
#define NULL ( (void *) 0)



void init_gpio(GpioObject_t* t)
{
}

GpioRegisters_t* get_registers(GpioObject_t* t)
{
    return NULL;
}


void set_moder(GpioObject_t* t, ModerTypes_t moder)
{
}

void into_af(GpioObject_t* t, unsigned int af_number)
{
}

void set_otyper(GpioObject_t* t, OutputTypes_t otype)
{
}



void toggle_output_pin(GpioObject_t* t)
{
}


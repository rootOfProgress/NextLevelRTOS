#include "gpio/gpio.h"
#include "rcc.h"
#define NULL ( (void *) 0)

const unsigned int* GPIO_A_BASE = (unsigned int*) 0x48000000;

void init_gpio(__attribute__((unused)) GpioObject_t* t)
{
}

GpioRegisters_t* get_registers(__attribute__((unused)) GpioObject_t* t)
{
  return NULL;
}

void set_moder(__attribute__((unused)) GpioObject_t* t, __attribute__((unused)) ModerTypes_t moder)
{
}

void into_af(__attribute__((unused)) GpioObject_t* t, __attribute__((unused)) unsigned int af_number)
{
}

void set_otyper(__attribute__((unused)) GpioObject_t* t, __attribute__((unused)) OutputTypes_t otype)
{
}

void toggle_output_pin(__attribute__((unused)) GpioObject_t* t)
{
}

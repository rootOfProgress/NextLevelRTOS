#include "gpio.h"
#include "rcc.h"

void init_gpio(GpioActions_t* gpio_actions)
{
    Rcc_t* rcc_regs = (Rcc_t*) BASE_ADRESS;
    switch (gpio_actions->gpio_object->port)
    {
    case 'A':
        *(rcc_regs->ahbenr) = *(rcc_regs->ahbenr) | 17;
        gpio_actions->gpio_object->base_adress = GPIO_A_BASE;
        break;
    case 'B':
        *(rcc_regs->ahbenr) = *(rcc_regs->ahbenr) | 18;
        break;   
    case 'C':
        *(rcc_regs->ahbenr) = *(rcc_regs->ahbenr) | 19;
        break;  
    case 'D':
        *(rcc_regs->ahbenr) = *(rcc_regs->ahbenr) | 20;
        break;  
    default:
        break;
    } 

    gpio_actions->toggle = toggle_output_pin;
}

void toggle_output_pin(GpioObject_t* t)
{
    switch (t->port)
    {
    case 'A':
        Gpio_t* gpio_regs = (Gpio_t*) GPIO_A_BASE;
        *gpio_regs->odr = *gpio_regs->odr & ~(1 << t->port);
        break;
    
    default:
        break;
    }
}

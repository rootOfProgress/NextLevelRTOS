#include "gpio.h"
#include "rcc.h"
#include "lang.h"


void init_gpio(GpioActions_t* gpio_actions)
{
    RccRegisterMap_t* rcc_regs = (RccRegisterMap_t*) RCC_BASE_ADRESS;

    switch (gpio_actions->gpio_object->port)
    {
    case 'A':
        WRITE_REGISTER(rcc_regs->ahbenr, READ_REGISTER(rcc_regs->ahbenr) | 17);
        gpio_actions->gpio_object->base_adress = (unsigned int*) GPIO_A_BASE;
        break;
    case 'B':
        WRITE_REGISTER(rcc_regs->ahbenr, READ_REGISTER(rcc_regs->ahbenr) | 18);
        gpio_actions->gpio_object->base_adress = (unsigned int*) GPIO_B_BASE;
        break;   
    case 'C':
        WRITE_REGISTER(rcc_regs->ahbenr, READ_REGISTER(rcc_regs->ahbenr) | 19);
        gpio_actions->gpio_object->base_adress = (unsigned int*) GPIO_C_BASE;
        break;  
    case 'D':
        WRITE_REGISTER(rcc_regs->ahbenr, READ_REGISTER(rcc_regs->ahbenr) | 20);
        gpio_actions->gpio_object->base_adress = (unsigned int*) GPIO_D_BASE;
        break;  
    default:
        break;
    } 

    gpio_actions->toggle = toggle_output_pin;
}

void toggle_output_pin(GpioObject_t* t)
{
    Gpio_t* gpio_regs;
    switch (t->port)
    {
    case 'A':
        gpio_regs = (Gpio_t*) ((unsigned int*) GPIO_A_BASE);
        break;
    case 'B':
        gpio_regs = (Gpio_t*) ((unsigned int*) GPIO_B_BASE);
        break;   
    case 'C':
        gpio_regs = (Gpio_t*) ((unsigned int*) GPIO_C_BASE);
        break;  
    case 'D':
        gpio_regs = (Gpio_t*) ((unsigned int*) GPIO_D_BASE);
        break; 
    default:
        return;
    }
    WRITE_REGISTER(gpio_regs->odr, READ_REGISTER(gpio_regs->odr) & ~(1 << t->port));
}

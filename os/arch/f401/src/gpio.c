#include "gpio.h"
#include "rcc.h"

#define READ_REGISTER(addr)     (*(volatile unsigned int *) (addr))
#define WRITE_REGISTER(addr, val) ((*(volatile unsigned int *) (addr)) = (unsigned int) (val))

void gpio_set_registermap(GpioObject_t* t)
{
    switch (t->port)
    {
    case 'A':
        t->gpio_regs = (GpioRegisters_t*) ((unsigned int*) GpioABase);
        break;
    case 'B':
        t->gpio_regs = (GpioRegisters_t*) ((unsigned int*) GpioBBase);
        break;
    case 'C':
        t->gpio_regs = (GpioRegisters_t*) ((unsigned int*) GpioCBase);
        break;
    case 'D':
        t->gpio_regs = (GpioRegisters_t*) ((unsigned int*) GpioDBase);
        break;
    default:
        t->gpio_regs = (GpioRegisters_t*) ((unsigned int*) GpioABase);
        break;
    }
}

void init_gpio(GpioObject_t* gpio_object)
{
    RccRegisterMap_t* rcc_regs = (RccRegisterMap_t*) RccBaseAdress;

    switch (gpio_object->port)
    {
    case 'A':
        WRITE_REGISTER(&rcc_regs->ahb1enr, READ_REGISTER(&rcc_regs->ahb1enr) | (1 << 0));
        break;
    case 'B':
        WRITE_REGISTER(&rcc_regs->ahb1enr, READ_REGISTER(&rcc_regs->ahb1enr) | (1 << 1));
        break;   
    case 'C':
        WRITE_REGISTER(&rcc_regs->ahb1enr, READ_REGISTER(&rcc_regs->ahb1enr) | (1 << 2));
        break;  
    case 'D':
        WRITE_REGISTER(&rcc_regs->ahb1enr, READ_REGISTER(&rcc_regs->ahb1enr) | (1 << 3));
        break;  
    default:
        break;
    } 
    gpio_set_registermap(gpio_object);
}

void set_moder(GpioObject_t* t, ModerTypes_t moder)
{
    WRITE_REGISTER(&t->gpio_regs->moder, READ_REGISTER(&t->gpio_regs->moder) & ~(0b11 << t->pin * 2));

    switch (moder)
    {
    case InputMode:
        WRITE_REGISTER(&t->gpio_regs->moder, READ_REGISTER(&t->gpio_regs->moder) | (INPUT << t->pin * 2));
        break;
    case GeneralPurposeOutputMode:
        WRITE_REGISTER(&t->gpio_regs->moder, READ_REGISTER(&t->gpio_regs->moder) | (GENERALPURPOSEOUTPUT << t->pin * 2));
        break;   
    case AlternateFunctionMode:
        WRITE_REGISTER(&t->gpio_regs->moder, READ_REGISTER(&t->gpio_regs->moder) | (ALTERNATE << t->pin * 2));
        break;
    case AnalogMode:
        WRITE_REGISTER(&t->gpio_regs->moder, READ_REGISTER(&t->gpio_regs->moder) | (ANALOG << t->pin * 2));
        break; 
    default:
        break;
    }
}

void into_af(GpioObject_t* t, unsigned int af_number)
{
    if (t->pin < 8)
    {
        WRITE_REGISTER(&t->gpio_regs->afrl, READ_REGISTER(&t->gpio_regs->afrl) & ~(0xF << (t->pin * 4)));    
        WRITE_REGISTER(&t->gpio_regs->afrl, READ_REGISTER(&t->gpio_regs->afrl) | (af_number << (t->pin * 4)));
    }
    else
    {
        unsigned int pin  = t->pin - 8;
        WRITE_REGISTER(&t->gpio_regs->afrh, READ_REGISTER(&t->gpio_regs->afrh) & ~(0xF << (pin * 4)));    
        WRITE_REGISTER(&t->gpio_regs->afrh, READ_REGISTER(&t->gpio_regs->afrh) | (af_number << (pin * 4)));
    }
}

void set_speed(GpioObject_t* t, SpeedModes_t speed)
{
    WRITE_REGISTER(&t->gpio_regs->ospeedr, READ_REGISTER(&t->gpio_regs->ospeedr) & ~(0b11 << (t->pin * 2)));    
    WRITE_REGISTER(&t->gpio_regs->ospeedr, READ_REGISTER(&t->gpio_regs->ospeedr) | (speed << (t->pin * 2)));    
}


void set_pupdr(GpioObject_t* t, PullTypes_t pull_type)
{
    WRITE_REGISTER(&t->gpio_regs->pupdr, READ_REGISTER(&t->gpio_regs->pupdr) & ~(0b11 << (t->pin * 2)));    
    WRITE_REGISTER(&t->gpio_regs->pupdr, READ_REGISTER(&t->gpio_regs->pupdr) | (pull_type << (t->pin * 2)));    
}

void set_otyper(GpioObject_t* t, OutputTypes_t otype)
{
    WRITE_REGISTER(&t->gpio_regs->otyper, READ_REGISTER(&t->gpio_regs->otyper) & ~(0b11 << (t->pin)));
    WRITE_REGISTER(&t->gpio_regs->otyper, READ_REGISTER(&t->gpio_regs->otyper) | (otype << (t->pin)));
}

void toggle_output_pin(GpioObject_t* t)
{
    WRITE_REGISTER(&t->gpio_regs->odr, READ_REGISTER(&t->gpio_regs->odr) & ~(1 << t->port));
}

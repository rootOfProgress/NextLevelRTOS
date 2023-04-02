#include "gpio.h"
#include "rcc.h"

#define READ_REGISTER(addr)     (*(volatile unsigned int *) (addr))
#define WRITE_REGISTER(addr, val) ((*(volatile unsigned int *) (addr)) = (unsigned int) (val))

void init_gpio(GpioObject_t* gpio_object)
{
    RccRegisterMap_t* rcc_regs = (RccRegisterMap_t*) RccBaseAdress;
    switch (gpio_object->port)
    {
    case 'A':
        WRITE_REGISTER(&rcc_regs->ahb1enr, READ_REGISTER(&rcc_regs->ahb1enr) | (1 << 0));
        gpio_object->base_adress = (unsigned int*) GPIO_A_BASE;
        break;
    case 'B':
        WRITE_REGISTER(&rcc_regs->ahb1enr, READ_REGISTER(&rcc_regs->ahb1enr) | (1 << 1));
        gpio_object->base_adress = (unsigned int*) GPIO_B_BASE;
        break;   
    case 'C':
        WRITE_REGISTER(&rcc_regs->ahb1enr, READ_REGISTER(&rcc_regs->ahb1enr) | (1 << 2));
        gpio_object->base_adress = (unsigned int*) GPIO_C_BASE;
        break;  
    case 'D':
        WRITE_REGISTER(&rcc_regs->ahb1enr, READ_REGISTER(&rcc_regs->ahb1enr) | (1 << 3));
        gpio_object->base_adress = (unsigned int*) GPIO_D_BASE;
        break;  
    default:
        break;
    } 
}

void set_moder(GpioObject_t* t, ModerTypes_t moder)
{
    GpioRegisters_t* gpio_regs = get_registers(t);
    WRITE_REGISTER(&gpio_regs->moder, READ_REGISTER(&gpio_regs->moder) & ~(0b11 << t->pin * 2));

    switch (moder)
    {
    case InputMode:
        WRITE_REGISTER(&gpio_regs->moder, READ_REGISTER(&gpio_regs->moder) | (INPUT << t->pin * 2));
        break;
    case GeneralPurposeOutputMode:
        WRITE_REGISTER(&gpio_regs->moder, READ_REGISTER(&gpio_regs->moder) | (GENERALPURPOSEOUTPUT << t->pin * 2));
        break;   
    case AlternateFunctionMode:
        WRITE_REGISTER(&gpio_regs->moder, READ_REGISTER(&gpio_regs->moder) | (ALTERNATE << t->pin * 2));
        break;
    case AnalogMode:
        WRITE_REGISTER(&gpio_regs->moder, READ_REGISTER(&gpio_regs->moder) | (ANALOG << t->pin * 2));
        break; 
    default:
        break;
    }
}

void into_af(GpioObject_t* t, unsigned int af_number)
{
    GpioRegisters_t* gpio_regs = get_registers(t);
    if (t->pin < 8)
    {
        WRITE_REGISTER(&gpio_regs->afrl, READ_REGISTER(&gpio_regs->afrl) & ~(0xF << (t->pin * 4)));    
        WRITE_REGISTER(&gpio_regs->afrl, READ_REGISTER(&gpio_regs->afrl) | (af_number << (t->pin * 4)));
    }
    else
    {
        unsigned int pin  = t->pin - 8;
        WRITE_REGISTER(&gpio_regs->afrh, READ_REGISTER(&gpio_regs->afrh) & ~(0xF << (pin * 4)));    
        WRITE_REGISTER(&gpio_regs->afrh, READ_REGISTER(&gpio_regs->afrh) | (af_number << (pin * 4)));
    }
}

void set_speed(GpioObject_t* t, SpeedModes_t speed)
{
    GpioRegisters_t* gpio_regs = get_registers(t);
    WRITE_REGISTER(&gpio_regs->ospeedr, READ_REGISTER(&gpio_regs->ospeedr) & ~(11 << (t->pin * 2)));    
    WRITE_REGISTER(&gpio_regs->ospeedr, READ_REGISTER(&gpio_regs->ospeedr) | (speed << (t->pin * 2)));    
}


void set_pupdr(GpioObject_t* t, PullTypes_t pull_type)
{
    GpioRegisters_t* gpio_regs = get_registers(t);
    WRITE_REGISTER(&gpio_regs->pupdr, READ_REGISTER(&gpio_regs->pupdr) & ~(11 << (t->pin * 2)));    
    WRITE_REGISTER(&gpio_regs->pupdr, READ_REGISTER(&gpio_regs->pupdr) | (pull_type << (t->pin * 2)));    
}

void set_otyper(GpioObject_t* t, OutputTypes_t otype)
{
    GpioRegisters_t* gpio_regs = get_registers(t);
    WRITE_REGISTER(&gpio_regs->otyper, READ_REGISTER(&gpio_regs->otyper) & ~(otype << (t->pin)));
    WRITE_REGISTER(&gpio_regs->otyper, READ_REGISTER(&gpio_regs->otyper) | (otype << (t->pin)));
}

void toggle_output_pin(GpioObject_t* t)
{
    GpioRegisters_t* gpio_regs = get_registers(t);

    WRITE_REGISTER(&gpio_regs->odr, READ_REGISTER(&gpio_regs->odr) & ~(1 << t->port));
}

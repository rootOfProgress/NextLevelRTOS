#include "gpio.h"
#include "rcc.h"
#include "lang.h"

void init_gpio(GpioObject_t* gpio_object)
{
  RccRegisterMap_t* rcc_regs = (RccRegisterMap_t*) RCC_BASE_ADRESS;

  switch (gpio_object->port)
  {
  case 'A':
    WRITE_REGISTER(&rcc_regs->ahbenr, READ_REGISTER(&rcc_regs->ahbenr) | (1 << 17));
    gpio_object->base_adress = (unsigned int*) GPIO_A_BASE;
    break;
  case 'B':
    WRITE_REGISTER(&rcc_regs->ahbenr, READ_REGISTER(&rcc_regs->ahbenr) | (1 << 18));
    gpio_object->base_adress = (unsigned int*) GPIO_B_BASE;
    break;
  case 'C':
    WRITE_REGISTER(&rcc_regs->ahbenr, READ_REGISTER(&rcc_regs->ahbenr) | (1 << 19));
    gpio_object->base_adress = (unsigned int*) GPIO_C_BASE;
    break;
  case 'D':
    WRITE_REGISTER(&rcc_regs->ahbenr, READ_REGISTER(&rcc_regs->ahbenr) | (1 << 20));
    gpio_object->base_adress = (unsigned int*) GPIO_D_BASE;
    break;
  default:
    break;
  }
}

GpioRegisters_t* get_registers(GpioObject_t* t)
{
  switch (t->port)
  {
  case 'A':
    return (GpioRegisters_t*) ((unsigned int*) GPIO_A_BASE);
  case 'B':
    return (GpioRegisters_t*) ((unsigned int*) GPIO_B_BASE);
  case 'C':
    return (GpioRegisters_t*) ((unsigned int*) GPIO_C_BASE);
  case 'D':
    return (GpioRegisters_t*) ((unsigned int*) GPIO_D_BASE);
  default:
    // dummy
    return (GpioRegisters_t*) ((unsigned int*) GPIO_A_BASE);
  }
}

void set_moder(GpioObject_t* t, ModerTypes_t moder)
{
  GpioRegisters_t* gpio_regs = get_registers(t);

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
    // @todo: do not delete
    // if ((t->pin - 8) < 0 || (t->pin - 8) > 7)
    // return;
    // unsigned int pin  = t->pin - 8;
    // WRITE_REGISTER(&gpio_regs->afrh, READ_REGISTER(&gpio_regs->afrh) & ~(0xF << (pin * 4)));

    // @todo: WARNING HARDCODED!
    // WRITE_REGISTER(&gpio_regs->afrh, af_number << (pin * 4));
    WRITE_REGISTER(&gpio_regs->afrh, 0x00000770);

  }
}

void set_otyper(GpioObject_t* t, OutputTypes_t otype)
{
  GpioRegisters_t* gpio_regs = get_registers(t);
  switch (otype)
  {
  case PushPull:
    WRITE_REGISTER(&gpio_regs->otyper, READ_REGISTER(&gpio_regs->otyper) & ~(1 << (t->pin)));
    break;
  case OpenDrain:
    WRITE_REGISTER(&gpio_regs->otyper, READ_REGISTER(&gpio_regs->otyper) | (1 << (t->pin)));
    break;
  default:
    break;
  }
}

void toggle_output_pin(GpioObject_t* t)
{
  GpioRegisters_t* gpio_regs = get_registers(t);

  WRITE_REGISTER(&gpio_regs->odr, READ_REGISTER(&gpio_regs->odr) & ~(1 << t->port));
}

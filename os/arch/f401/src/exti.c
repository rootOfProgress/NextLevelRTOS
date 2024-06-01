#include "exti.h"
#include "lang.h"
#include "rcc.h"

void NO_OPT exti_activate_ir_line(GpioObject_t* gpio)
{
  ExtiRegisterMap_t* exti_registers = (ExtiRegisterMap_t*) ExtiBaseRegister;
  WRITE_REGISTER(&exti_registers->imr1, READ_REGISTER(&exti_registers->imr1) | 1 << gpio->pin);
  WRITE_REGISTER(&exti_registers->emr1, READ_REGISTER(&exti_registers->emr1) | 1 << gpio->pin);
}

void exti_detect_rising_edge(GpioObject_t* gpio)
{
  ExtiRegisterMap_t* exti_registers = (ExtiRegisterMap_t*) ExtiBaseRegister;
  WRITE_REGISTER(&exti_registers->rtsr1, READ_REGISTER(&exti_registers->rtsr1) | 1 << gpio->pin);
}

void exti_detect_falling_edge(GpioObject_t* gpio)
{
  ExtiRegisterMap_t* exti_registers = (ExtiRegisterMap_t*) ExtiBaseRegister;
  WRITE_REGISTER(&exti_registers->ftsr1, READ_REGISTER(&exti_registers->ftsr1) | 1 << gpio->pin);
}

void exti_acknowledge_interrupt(GpioObject_t* gpio)
{
  ExtiRegisterMap_t* exti_registers = (ExtiRegisterMap_t*) ExtiBaseRegister;
  WRITE_REGISTER(&exti_registers->pr1, READ_REGISTER(&exti_registers->pr1) | 1 << gpio->pin);
}
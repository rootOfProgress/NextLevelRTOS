#include "exti.h"
#include "lang.h"
#include "rcc.h"


void NO_OPT exti_activate_ir_line(unsigned int exti_number)
{
    ExtiRegisterMap_t* exti_registers = (ExtiRegisterMap_t*) ExtiBaseRegister;
    WRITE_REGISTER(&exti_registers->imr1, READ_REGISTER(&exti_registers->imr1) | 1 << exti_number);
    WRITE_REGISTER(&exti_registers->emr1, READ_REGISTER(&exti_registers->emr1) | 1 << exti_number);
}

void exti_detect_rising_edge(unsigned int exti_number)
{
    ExtiRegisterMap_t* exti_registers = (ExtiRegisterMap_t*) ExtiBaseRegister;
    WRITE_REGISTER(&exti_registers->rtsr1, READ_REGISTER(&exti_registers->rtsr1) | 1 << exti_number);
}

void exti_detect_falling_edge(unsigned int exti_number)
{
    ExtiRegisterMap_t* exti_registers = (ExtiRegisterMap_t*) ExtiBaseRegister;
    WRITE_REGISTER(&exti_registers->ftsr1, READ_REGISTER(&exti_registers->ftsr1) | 1 << exti_number);
}

void exti_acknowledge_interrupt(unsigned int exti_number)
{
    ExtiRegisterMap_t* exti_registers = (ExtiRegisterMap_t*) ExtiBaseRegister;
    WRITE_REGISTER(&exti_registers->pr1, READ_REGISTER(&exti_registers->pr1) | 1 << exti_number);
}
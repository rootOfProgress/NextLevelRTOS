#ifndef EXTI_H
#define EXTI_H

enum {
    ExtiBaseRegister = 0x40013C00
};

typedef struct ExtiRegisterMap {
    unsigned int imr1;
    unsigned int emr1;
    unsigned int rtsr1;
    unsigned int ftsr1;
    unsigned int swier1;
    unsigned int pr1;
    unsigned int imr2;
    unsigned int emr2;
    unsigned int rtsr2;
    unsigned int ftsr2;
    unsigned int swier2;
    unsigned int pr2;
} ExtiRegisterMap_t;

void exti_activate_ir_line(unsigned int line_number);
void exti_detect_rising_edge(unsigned int line_number);
void exti_detect_falling_edge(unsigned int line_number);

#endif
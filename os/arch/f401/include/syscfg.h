#ifndef SYSCFG_H
#define SYSCFG_H

enum { SysCfgBaseAdress = 0x40013800 };

typedef struct SysCfgRegisterMap {
    unsigned int memrmp;
    unsigned int pmc;
    unsigned int exticr1;
    unsigned int exticr2;
    unsigned int exticr3;
    unsigned int exticr4;
    unsigned int cmpcr;
} SysCfgRegisterMap_t;

void syscfg_enable_clock(void);
void syscfg_exti_config_0_3(unsigned int pin, unsigned int exti_number);


#endif
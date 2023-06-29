#ifndef RCC_H
#define RCC_H

enum { RccBaseAdress = 0x40023800 };

#define I2C1EN 21
#define SPI1EN 12

typedef union rcc_cr
{
    struct
    {
        unsigned int hsion : 1, hsirdy : 1, : 1, hsitrim : 5, hsical : 8,
                     hseon : 1, hserdy : 1, hsebyp : 1, csson : 1, : 4, pllon : 1, pllrdy : 1, plli2son : 1, plli2rdy : 1, : 4; 
    } cr;
    unsigned int raw;
} rcc_cr_t;

typedef union rcc_pllcfgr
{
    struct
    {
        unsigned int pllm0 : 1,
                     pllm1 : 1,
                     pllm2 : 1,   
                     pllm3 : 1,   
                     pllm4 : 1,   
                     pllm5 : 1,
                     plln  : 9,
                           : 1,
                     pllp0 : 1,
                     pllp1 : 1,       
                           : 4,
                     pllsrc: 1,
                           : 1,
                     pllq0 : 1,
                     pllq1 : 1,
                     pllq2 : 1,
                     pllq3 : 1,
                           : 4;
    } pllcfgr;
    unsigned int raw;
} rcc_pllcfgr_t;

typedef struct Rcc {
    rcc_cr_t cr;
    rcc_pllcfgr_t pllcfgr;
    unsigned int cfgr;
    unsigned int cir; // 0x0c
    unsigned int ahb1rstr;
    unsigned int ahb2rstr; // 0x14
    unsigned int reserved0; // 0x18
    unsigned int reserved1; // 0x1C
    unsigned int apb1rstr;
    unsigned int apb2rstr;
    unsigned int reserved2; // 0x28
    unsigned int reserved3; // 0x2C
    unsigned int ahb1enr;
    unsigned int ahb2enr;
    unsigned int reserved4; // 0x38
    unsigned int reserved5; // 0x3C
    unsigned int apb1enr;
    unsigned int apb2enr;
    unsigned int reserved6;
    unsigned int reserved7; // 0x4C
    unsigned int ahb1lpenr;
    unsigned int ahb2lpenr; // 0x54
    unsigned int reserved8;
    unsigned int reserved9; // 0x5c
    unsigned int apb1lpenr;
    unsigned int apb2lpenr;
    unsigned int reserved10;
    unsigned int reserved11; // 0x6c
    unsigned int bdcr;
    unsigned int csr;
    unsigned int reserved12;
    unsigned int reserved13;
    unsigned int sscgrg;
    unsigned int plli2scfgr;
    unsigned int dckcfgr;
} RccRegisterMap_t;

#endif
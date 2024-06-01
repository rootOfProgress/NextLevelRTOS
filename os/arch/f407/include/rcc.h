#ifndef RCC_H
#define RCC_H

extern unsigned int RCC_BASE_ADRESS;

// for stm32f4x...
// #[repr(C)]
typedef struct Rcc
{
  unsigned int* cr;
  unsigned int* pllcfgr;
  unsigned int* cfgr;
  unsigned int* cir;
  unsigned int* ahb1rstr;
  unsigned int* ahb2rstr;
  unsigned int* ahb3rstr;
  unsigned int* reserved0;
  unsigned int* apb1rstr;
  unsigned int* apb2rstr;
  unsigned int* reserved1;
  unsigned int* reserved2;
  unsigned int* ahb1enr;
  unsigned int* ahb2enr;
  unsigned int* ahb3enr;
  unsigned int* reserved3;
  unsigned int* apb1enr;
  unsigned int* apb2enr;
  unsigned int* reserved4;
  unsigned int* reserved5;
  unsigned int* ahb1lpenr;
  unsigned int* ahb2lpenr;
  unsigned int* ahb3lpenr;
  unsigned int* reserved6;
  unsigned int* apb1lpenr;
  unsigned int* apb2lpenr;
  unsigned int* reserved7;
  unsigned int* reserved8;
  unsigned int* bdcr;
  unsigned int* csr;
  unsigned int* reserved9;
  unsigned int* reserved10;
  unsigned int* sscgrg;
  unsigned int* plli2scfgr;
} RccRegisterMap_t;

#endif
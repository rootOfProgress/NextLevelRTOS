#ifndef RCC_H
#define RCC_H

#define BASE_ADRESS 0x40021000

typedef struct RCC
{
  unsigned int* cr;
  unsigned int* cfgr;
  unsigned int* cir;
  unsigned int* apb2rstr;
  unsigned int* apb1rstr;
  unsigned int* ahbenr;
  unsigned int* apb2enr;
  unsigned int* apb1enr;
  unsigned int* bdcr;
  unsigned int* csr;
  unsigned int* ahbrstr;
  unsigned int* cfgr2;
  unsigned int* cfgr3;
} RccRegisterMap_t;

#endif
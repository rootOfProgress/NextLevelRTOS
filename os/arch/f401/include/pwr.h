#ifndef PWR_H
#define PWR_H

typedef struct PWRRegisterMap
{
  unsigned int CR;
  unsigned int CSR;
} PWRRegisterMap_t;

enum 
{
  PWRBaseAdress = 0x40007000
};

#endif
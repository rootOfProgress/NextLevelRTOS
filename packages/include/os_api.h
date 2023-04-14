#ifndef OS_API_H
#define OS_API_H
#include "addresses.h"

#define BREAK asm("bkpt") 
#define READ_REGISTER(addr)     (*(volatile unsigned int *) (addr))
#define WRITE_REGISTER(addr, val) ((*(volatile unsigned int *) (addr)) = (unsigned int) (val))

unsigned int* (*allocate)(unsigned int) = (unsigned int* (*)(unsigned int size)) allocate_addr;
unsigned int (*deallocate)(unsigned int*) = (unsigned int (*)(unsigned int* address)) deallocate_addr;

#endif
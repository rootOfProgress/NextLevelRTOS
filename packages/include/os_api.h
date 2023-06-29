#ifndef OS_API_H
#define OS_API_H
#include "addresses.h"

#define BREAK asm("bkpt") 
#define READ_REGISTER(addr)     (*(volatile unsigned int *) (addr))
#define WRITE_REGISTER(addr, val) ((*(volatile unsigned int *) (addr)) = (unsigned int) (val))

static __attribute__((unused)) unsigned int* (*allocate)(unsigned int) = (unsigned int* (*)(unsigned int size)) allocate_addr;
static __attribute__((unused)) unsigned int (*deallocate)(unsigned int*) = (unsigned int (*)(unsigned int* address)) deallocate_addr;
static __attribute__((unused)) unsigned int (*sleep)(unsigned int) = (unsigned int (*)(unsigned int time_to_sleep)) task_sleep_addr;
static __attribute__((unused)) unsigned int (*create_task)(void (*task_addr)(), unsigned int) = (unsigned int (*)(void (*task_addr)() , unsigned int external)) create_task_addr;
static __attribute__((unused)) unsigned int (*join_task)(unsigned int) = (unsigned int (*)(unsigned int pid_to_wait_for)) join_task_addr;
static __attribute__((unused)) unsigned int (*memset_byte)(void*, unsigned int, char) = (unsigned int (*)(void* dest, unsigned int number_of_bytes, char pattern)) memset_byte_addr;

#endif
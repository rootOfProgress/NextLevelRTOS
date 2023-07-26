#ifndef OS_API_H
#define OS_API_H
#include "addresses.h"

// @todo forwarddeclaration is a workaround!!
typedef struct GpioObject GpioObject_t;

#define BREAK asm("bkpt") 
#define READ_REGISTER(addr)     (*(volatile unsigned int *) (addr))
#define WRITE_REGISTER(addr, val) ((*(volatile unsigned int *) (addr)) = (unsigned int) (val))

static __attribute__((unused)) unsigned int* (*allocate)(unsigned int) = (unsigned int* (*)(unsigned int size)) allocate_addr;
static __attribute__((unused)) unsigned int (*deallocate)(unsigned int*) = (unsigned int (*)(unsigned int* address)) deallocate_addr;
static __attribute__((unused)) unsigned int (*sleep)(unsigned int) = (unsigned int (*)(unsigned int time_to_sleep)) task_sleep_addr;
static __attribute__((unused)) unsigned int (*create_task)(void (*task_addr)(), unsigned int) = (unsigned int (*)(void (*task_addr)() , unsigned int external)) create_task_addr;
static __attribute__((unused)) unsigned int (*join_task)(unsigned int) = (unsigned int (*)(unsigned int pid_to_wait_for)) join_task_addr;
static __attribute__((unused)) unsigned int (*memset_byte)(void*, unsigned int, char) = (unsigned int (*)(void* dest, unsigned int number_of_bytes, char pattern)) memset_byte_addr;
static __attribute__((unused)) void (*link_exti_src)(void (*isr_callback)(), GpioObject_t *gpio_obj) = (unsigned int (*)(void (*isr_callback)(), GpioObject_t *gpio_obj)) link_exti_src_addr;
static __attribute__((unused)) void (*execute_priviledged)(void (*target_fn)()) = (unsigned int (*)(void (*task_addr)() )) execute_priviledged_addr;


#endif
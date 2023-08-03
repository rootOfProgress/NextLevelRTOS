#ifndef OS_API_H
#define OS_API_H
#include "addresses.h"

// @todo forwarddeclaration is a workaround!!
typedef struct GpioObject GpioObject_t;

#define INTERRUPT __attribute__((interrupt))
#define ISR __attribute__ ((interrupt ("IRQ")))
#define UNUSED __attribute__((unused))
#define USED __attribute__((used))
#define NO_OPT __attribute__((optimize("O0")))
#define NULL ( (void *) 0)
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
static __attribute__((unused)) void (*link_adc_isr)(void (*isr_callback)()) = (unsigned int (*)(void (*isr_callback)())) link_adc_src_addr;

#endif
#ifndef EXCEPTION_SVC_H
#define EXCEPTION_SVC_H


typedef enum {
    execPspTask = 0,
    printMsg,
    yieldTask,
    std,
    ste,
    reset,
    execPriv,
    wakeupPid,
    setExtIoCallback = 8,
    getIoBuffer,
    wakeupIoHandler,
} TrapType_t;


static inline __attribute__((always_inline)) NO_OPT void svcall( unsigned int svc_number)
{
__asm volatile ("mov r6, %[input]"
    : // no C variable outputs
    : [input] "r" (svc_number)
    : "r6"   // tell the compiler r9 is modified
      );
__asm volatile ("svc 0\n");
}

static inline __attribute__((always_inline)) NO_OPT void svcall_param(unsigned int svc_number,unsigned int parameter)
{
__asm volatile ("mov r6, %[input]"
    : // no C variable outputs
    : [input] "r" (svc_number)
    : "r9"   // tell the compiler r9 is modified
      );
__asm volatile ("mov r9, %[input]"
    : // no C variable outputs
    : [input] "r" (parameter)
    : "r9"   // tell the compiler r9 is modified
      );
__asm volatile ("svc 0\n");

}

#endif

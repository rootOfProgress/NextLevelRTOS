#include <stdint.h>
// #include "../include/reg.h"
#include "memory.h"
#include "../include/exception.h"
void reset_handler(void);
extern void main_init(void);
extern void hostIfISR(void);
extern void uart_isr_handler(void);
extern void SVCall(unsigned int, unsigned int);
extern void PendSV(void);

extern unsigned int _sidata;
extern unsigned int _sdata;
extern unsigned int _edata;
extern unsigned int _sbss;
extern unsigned int _ebss;
extern unsigned int stack_top;

void reset_handler(void)
{
    unsigned int max = (unsigned int) &_ebss;
    if ((unsigned int) &_edata > max)
        max = (unsigned int) &_edata;
    if ((unsigned int) &_sidata > max)
        max = (unsigned int) &_sidata; 
    init_allocator( max );
    main_init();
}

void nmi_handler(void)
{
    while (1);
}

void memfault_handler(void)
{
    while (1);
}

void hardfault_handler(void)
{
    while (1);
}

void usage_fault(void)
{
    while (1);
}

void bar_handler(void)
{
    while (1);
}

__attribute((section(".isr_vector")))
uint32_t *isr_vectors[] =
{
    (uint32_t *) &stack_top,
    (uint32_t *) reset_handler,
    (uint32_t *) nmi_handler,
    (uint32_t *) hardfault_handler,
    (uint32_t *) memfault_handler,
    (uint32_t *) bar_handler,
    (uint32_t *) usage_fault,
    (uint32_t *) bar_handler,
    (uint32_t *) bar_handler,
    (uint32_t *) bar_handler,
    (uint32_t *) bar_handler,
    (uint32_t *) SVCall,
    (uint32_t *) bar_handler,
    (uint32_t *) bar_handler,
    (uint32_t *) PendSV,
    (uint32_t *) bar_handler,
    (uint32_t *) bar_handler,
    (uint32_t *) bar_handler,
    (uint32_t *) bar_handler,
    (uint32_t *) bar_handler,
    (uint32_t *) bar_handler,
    (uint32_t *) bar_handler,
    (uint32_t *) bar_handler,
    (uint32_t *) bar_handler,
    (uint32_t *) bar_handler,
    (uint32_t *) bar_handler,
    (uint32_t *) bar_handler,
    (uint32_t *) bar_handler,
    (uint32_t *) bar_handler,
    (uint32_t *) bar_handler,
    (uint32_t *) bar_handler,
    (uint32_t *) bar_handler,
    (uint32_t *) bar_handler,
    (uint32_t *) bar_handler,
    (uint32_t *) bar_handler,
    (uint32_t *) bar_handler,
    (uint32_t *) bar_handler,
    (uint32_t *) uart_isr_handler,
    (uint32_t *) uart_isr_handler,
    (uint32_t *) uart_isr_handler,
    (uint32_t *) uart_isr_handler,
    (uint32_t *) uart_isr_handler,
    (uint32_t *) uart_isr_handler,
    (uint32_t *) uart_isr_handler,
    (uint32_t *) uart_isr_handler,
    (uint32_t *) uart_isr_handler,
    (uint32_t *) uart_isr_handler,
    (uint32_t *) uart_isr_handler,
    (uint32_t *) uart_isr_handler,
    (uint32_t *) uart_isr_handler,
    (uint32_t *) uart_isr_handler,
    (uint32_t *) uart_isr_handler,
    (uint32_t *) uart_isr_handler,
    (uint32_t *) uart_isr_handler,
    (uint32_t *) uart_isr_handler,
};

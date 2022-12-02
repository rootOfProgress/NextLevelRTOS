#include <stdint.h>
#include "../include/reg.h"
#include "memory.h"
#include "../include/exception.h"

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
    init_allocator( (unsigned int) &_ebss);
    main_init();
}

void nmi_handler(void)
{
    while (1);
}

void hardfault_handler(void)
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
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) SVCall,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) PendSV,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    (uint32_t *) 0,
    // (uint32_t *) uart_isr_handler,
};

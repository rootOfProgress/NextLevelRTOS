#include <stdint.h>
#include "../include/reg.h"
#include "../include/exception.h"

extern void main_init(void);
extern void hostIfISR(void);
extern void uart_isr_handler(void);
extern void SVCall(uint32_t *);

extern uint32_t _sidata;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;
extern uint32_t stack_top;

void reset_handler(void)
{
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
    (uint32_t *) uart_isr_handler,
};

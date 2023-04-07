#include <stdint.h>
// #include "../include/reg.h"
#include "memory.h"
#include "../include/exception.h"
#include "process/scheduler.h"
#include "hw/cpu.h"
void reset_handler(void);
extern void main_init(void);
extern void hostIfISR(void);
extern void uart_isr_handler(void);
extern void SVCall(unsigned int, unsigned int);
extern void PendSV(void);
extern void SysTick(void);
extern void dma2_stream5_ir_handler(void);
extern unsigned int _sidata;
extern unsigned int _sdata;
extern unsigned int _edata;
extern unsigned int _sbss;
extern unsigned int _ebss;
extern unsigned int stack_top;
extern unsigned int ram_size;

void reset_handler(void)
{
    unsigned int max = (unsigned int) &_ebss;
    if ((unsigned int) &_edata > max)
        max = (unsigned int) &_edata;
    if ((unsigned int) &_sidata > max)
        max = (unsigned int) &_sidata; 
    init_allocator( max , (unsigned int*) &ram_size );

    if (TEST)
    {
        WRITE_REGISTER(0x20000000, 0);
    }

    // CCR DIV_0_TRP , UNALIGN_ TRP
    WRITE_REGISTER(0xE000ED14, READ_REGISTER(0xE000ED14) | 3 << 3);

    // enable memfaults etc.
    volatile unsigned int *shcsr = (void *)0xE000ED24;
    *shcsr |= (0x1 << 16) | (0x1 << 17) | (0x1 << 18);

    if (HWFP)
    {
        __asm(
            "LDR.W R0, =0xE000ED88\n"
            "LDR R1, [R0]\n"
            "ORR R1, R1, #(0xF << 20)\n"
            "STR R1, [R0]\n"
            "DSB\n"
            "ISB\n"
        );
    }
    // WRITE_REGISTER(0xE000EF34, READ_REGISTER(0xE000EF34) & ~(1 << 30));
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


void  __attribute__((optimize("O3"))) hardfault_handler(void)
{
    process_stats.num_of_hardfaults++;
    // volatile uint32_t *USFR = (volatile uint32_t *)0xE000ED28;
    // volatile uint32_t foo = *USFR;
    unsigned int USFR = READ_REGISTER(0xE000ED28);

    // UNDEFINSTR 
    if (USFR & (1 + 16))
    {
        while (1)
        {
        }
        
    }

    // INVSTATE
    if (USFR & 1 << (1 + 16))
    {
        while (1)
        {
        }
        
    }

    // INVPC
    if (USFR & 1 << (2 + 16))
    {
        while (1)
        {
        }
        
    }

    // NOCP
    if (USFR & 1 << (3 + 16))
    {
        while (1)
        {
        }
        
    }

    // DIVBYZERO
    if (USFR & 1 << (9 + 16))
    {
        invalidate_current_task();
        switch_task();
        __asm volatile ("MOV R2, %[input_i]":: [input_i] "r" (((Tcb_t*) currently_running->data)->sp));
        __asm volatile (
        "LDMFD r2!, {r4-r11}\n"
        "MSR PSP, r2\n"
        );
    }
}

void  __attribute__((optimize("O0"))) usage_fault(void)
{
    unsigned int UFSR = READ_REGISTER(0xE000ED28) >> 16;
    if (UFSR & 1 << 2) // invpc
    {
        while (1)
        {
            /* code */
        }
        
    }
    while (1);
}


void busfault_handler(void)
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
    (uint32_t *) busfault_handler,
    (uint32_t *) usage_fault,
    (uint32_t *) bar_handler,
    (uint32_t *) bar_handler,
    (uint32_t *) bar_handler,
    (uint32_t *) bar_handler,
    (uint32_t *) SVCall,
    (uint32_t *) bar_handler,
    (uint32_t *) bar_handler,
    (uint32_t *) PendSV,
    (uint32_t *) SysTick,
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
    (uint32_t *) bar_handler,
    (uint32_t *) bar_handler,
    (uint32_t *) bar_handler,
    (uint32_t *) bar_handler,
    (uint32_t *) bar_handler,
    (uint32_t *) bar_handler,
    (uint32_t *) bar_handler,
    (uint32_t *) dma2_stream5_ir_handler, // wrong?
    (uint32_t *) bar_handler,
    (uint32_t *) bar_handler,
    (uint32_t *) bar_handler,
};

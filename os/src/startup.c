/*!
 *
 * @author: Benjamin Wilhelm
 * @brief: 
 * 
 *
 */

#include <stdint.h>
#include "exception.h"
#include "hw/cpu.h"
#include "memory.h"
#include "process/scheduler.h"
#include "runtime.h"

extern void dma2_stream5_ir_handler(void);              //!< Interrupt service routine for DMA2 Controller */
extern void pendsv_isr(void);                           //!< Interrupt service routine for pending SV Call */
extern KernelErrorCodes_t setup_kernel_runtime(void);   //!< Init function to load mandatory kernel modules */ 
extern void setup_devices(void);                        //!< Prepares the UART device for basic host communication */
extern void svcall_isr(unsigned int, unsigned int);     //!< Interrupt service routine for the CortexM4 supervisor call */
extern void systick_isr(void);                          //!< Interrupt service routine for the CortexM4 systick interrupt */
extern void tim3_isr_handler(void);                     //!< Interrupt service routine for the tim3 device */
extern void uart_isr_handler(void);                     //!< Interrupt service routine for the usart1 device */

extern unsigned int _edata;                             //!< Defined in linker script. Containts address of  */
extern unsigned int _ebss;                              //!< Defined in linker script. End of block starting symbol, containts static variables, located in RAM */
extern unsigned int _sidata;                            //!< Defined in linker script. Containts address */
extern unsigned int ram_size;                           //!< Defined in linker script. Needed for internal statistic purposes */
extern unsigned int stack_top;                          //!< Defined in linker script. Needed to reset main stack pointer after bootstrapping system */

void bootstrap(void)
{
    unsigned int max = (unsigned int) &_ebss;
    if ((unsigned int) &_edata > max)
        max = (unsigned int) &_edata;
    if ((unsigned int) &_sidata > max)
        max = (unsigned int) &_sidata; 

    init_allocator( max , (unsigned int*) &ram_size );

    // enable external interrupt sources for tim2/3
    WRITE_REGISTER(CPU_NVIC_ISER0, READ_REGISTER(CPU_NVIC_ISER0) | 1 << 28 | 1 << 29);

    // CCR DIV_0_TRP , UNALIGN_ TRP
    WRITE_REGISTER(CPU_SCB_CCR, READ_REGISTER(CPU_SCB_CCR) | 3 << 3);

    // enable memfaults etc.
    WRITE_REGISTER(CPU_SCB_SHCSR, READ_REGISTER(CPU_SCB_SHCSR) | (0x1 << 16) | (0x1 << 17) | (0x1 << 18));

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

    KernelErrorCodes_t kernel_err;
    kernel_err = setup_kernel_runtime();

    switch (kernel_err)
    {
        case SCHEDULER_INIT_FAILED:
            while (1)
            {
                /* code */
            }
            
            break;
        case TASK_CREATION_FAILED:
            while (1)
            {
                /* code */
            }
        case KERNEL_INIT_SUCCEDED:
        default:
            break;
    }

    setup_devices();

    __asm volatile ("mov r2, %[stack_top]":: [stack_top] "r" ((unsigned int) &stack_top));
    __asm__(\
        "msr msp, r2\n"\
    );
    SV_EXEC_PSP_TASK;
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

void  __attribute__((optimize("O0"))) usage_fault_handler(void)
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

void dummy(void)
{
    while (1)
    {}
    
}

__attribute((section(".isr_vector")))
unsigned int *isr_vectors[] =
{
    (unsigned int *) &stack_top,
    (unsigned int *) bootstrap,
    (unsigned int *) nmi_handler,
    (unsigned int *) hardfault_handler,
    (unsigned int *) memfault_handler,
    (unsigned int *) busfault_handler,
    (unsigned int *) usage_fault_handler,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) svcall_isr,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) pendsv_isr,
    (unsigned int *) systick_isr,
    (unsigned int *) dummy, // Pos0
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) uart_isr_handler,
    (unsigned int *) uart_isr_handler,
    (unsigned int *) uart_isr_handler,
    (unsigned int *) uart_isr_handler,
    (unsigned int *) uart_isr_handler,
    (unsigned int *) uart_isr_handler,
    (unsigned int *) uart_isr_handler,
    (unsigned int *) (void(*)()) (0x20005000 | 1), // tim2 isr
    (unsigned int *) tim3_isr_handler, // tim3 isr
    (unsigned int *) uart_isr_handler,
    (unsigned int *) uart_isr_handler,
    (unsigned int *) uart_isr_handler,
    (unsigned int *) uart_isr_handler,
    (unsigned int *) uart_isr_handler,
    (unsigned int *) uart_isr_handler,
    (unsigned int *) uart_isr_handler,
    (unsigned int *) uart_isr_handler,
    (unsigned int *) uart_isr_handler,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dma2_stream5_ir_handler, // position_68
    (unsigned int *) NULL,
    (unsigned int *) NULL,
    (unsigned int *) NULL,
};

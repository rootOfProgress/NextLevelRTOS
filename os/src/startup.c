/*!
 *
 * @author: Benjamin Wilhelm
 * @brief:
 *
 *
 */

#include "core/exception.h"
#include "core/exception_svc.h"
#include "hw/cpu.h"
#include "memory.h"
#include "process/scheduler.h"
#include "runtime.h"
#include "lang.h"
#include "startup.h"

extern void dma2_stream5_ir_handler(void);              //!< Interrupt service routine for DMA2 Controller */
extern void pendsv_isr(void);                           //!< Interrupt service routine for pending SV Call */
extern KernelErrorCodes_t setup_kernel_runtime(void);   //!< Init function to load mandatory kernel modules */
extern void setup_devices(void);                        //!< Prepares the UART device for basic host communication */
extern void svcall_isr(unsigned int, unsigned int);     //!< Interrupt service routine for the CortexM4 supervisor call */
extern void systick_isr(void);                          //!< Interrupt service routine for the CortexM4 systick interrupt */
extern void tim3_isr_handler(void);                     //!< Interrupt service routine for the tim3 device */
extern void uart_isr_handler(void);                     //!< Interrupt service routine for the usart1 device */
extern void exti0_isr_handler(void);
extern void exti1_isr_handler(void);
extern void exti2_isr_handler(void);
extern void exti3_isr_handler(void);
extern void exti4_isr_handler(void);
extern void exti9_5_isr_handler(void);

extern void usage_fault_handler(void);
extern void hardfault_handler(void);

extern void adc_isr_handler(void);
extern unsigned int _edata;                             //!< Defined in linker script. Containts address of  */
extern unsigned int _ebss;                              //!< Defined in linker script. End of block starting symbol, containts static variables, located in RAM */
extern unsigned int _sidata;                            //!< Defined in linker script. Containts address */
extern unsigned int ram_size;                           //!< Defined in linker script. Needed for internal statistic purposes */
extern unsigned int stack_top;                          //!< Defined in linker script. Needed to reset main stack pointer after bootstrapping system */

BootFlags_t boot_flags;

void bootstrap(void)
{
  unsigned int max = (unsigned int) &_ebss;

  if ((unsigned int) &_edata > max)
  {
    max = (unsigned int) &_edata;
  }
  if ((unsigned int) &_sidata > max)
  {
    max = (unsigned int) &_sidata;
  }

  // @todo
  // if ((READ_REGISTER((unsigned int*) 0x20000000) != 0x12345678))
  // {
  //   WRITE_REGISTER(0x20000000, 0x12345678);
  //   boot_flags.isColdStart = 1;
  //   boot_flags.reboot_type = IsColdStart;

  //   // enable external interrupt sources for tim2/3
  //   WRITE_REGISTER(CPU_NVIC_ISER0, READ_REGISTER(CPU_NVIC_ISER0) | 1 << 28 | 1 << 29);

  //   // CCR DIV_0_TRP , UNALIGN_ TRP
  //   WRITE_REGISTER(CPU_SCB_CCR, READ_REGISTER(CPU_SCB_CCR) | 3 << 3);

  //   // enable memfaults etc.
  //   WRITE_REGISTER(CPU_SCB_SHCSR, READ_REGISTER(CPU_SCB_SHCSR) | (0x1 << 16) | (0x1 << 17) | (0x1 << 18));

  // }
  // else
  // {
  //   boot_flags.isColdStart = 0;
  //   switch (boot_flags.reboot_type)
  //   {
  //   case RebootRequestedByOperator:
  //     // do something interesting here
  //     break;
  //   case RebootAfterHardFault:
  //     // do something interesting here
  //     break;
  //   case RebootAfterUsageFault:
  //     // do something interesting here
  //     break;
  //   default:
  //     break;
  //   }
  // }
  init_allocator( max, (unsigned int*) &ram_size );

  boot_flags.isColdStart = 1;
  boot_flags.reboot_type = IsColdStart;

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

  // @todo: Signal that something went wrong, e.g. led, outbuffer,...
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
  svcall(execPspTask);
}

void nmi_handler(void)
{
  while (1);
}

void memfault_handler(void)
{
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
    (unsigned int *) dummy, // Pos2
    (unsigned int *) dummy,
    (unsigned int *) dummy, // Pos4
    (unsigned int *) dummy,
    (unsigned int *) exti0_isr_handler, // Pos6
    (unsigned int *) exti1_isr_handler,
    (unsigned int *) exti2_isr_handler, // Pos8
    (unsigned int *) exti3_isr_handler,
    (unsigned int *) exti4_isr_handler,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) dummy,
    (unsigned int *) adc_isr_handler, // 18
    (unsigned int *) NULL, // 19, those are undefined
    (unsigned int *) NULL, // those are undefined 
    (unsigned int *) NULL, // those are undefined
    (unsigned int *) NULL, // 23, those are undefined
    (unsigned int *) exti9_5_isr_handler,
    (unsigned int *) NULL,
    (unsigned int *) NULL,
    (unsigned int *) NULL,
    (unsigned int *) NULL,
    (unsigned int *) (void(*)()) (0x20005000 | 1), // tim2 isr, pos 28
    (unsigned int *) tim3_isr_handler, // tim3 isr
    (unsigned int *) NULL,
    (unsigned int *) NULL,
    (unsigned int *) NULL,
    (unsigned int *) NULL,
    (unsigned int *) NULL,
    (unsigned int *) NULL,
    (unsigned int *) NULL,
    (unsigned int *) uart_isr_handler,
    (unsigned int *) NULL,
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

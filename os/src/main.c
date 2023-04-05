/*
 * OS Core specific includes
 */
#include "exception.h"
#include "process/scheduler.h"
#include "process/task.h"
#include "uart_common.h"
#include "memory.h"
#include "dma.h"

/*
 * Device specific includes
 */
#include "gpio.h"
#include "i2c.h"
#include "tim2_5.h"
#include "uart.h"

#include "test.h"
#include "math.h"
#include "runtime.h"

// void enable_exti0_cpu_irq()
// {
//   WRITE_REGISTER(0xE000E100, 0x1 << 6);
// }
unsigned int __errno = 0;

void setup_nvic_controller()
{
  // mask exti0 on line 6
  *((unsigned int*) 0x40013C0C) = *((unsigned int*) 0x40013C0C) << 6;

  // line 6 triggers on rising edge
  *((unsigned int*) 0x40013C08) = *((unsigned int*) 0x40013C08) << 6;

  // line 6 triggers on falling edge
  *((unsigned int*) 0x40013C0C) = *((unsigned int*) 0x40013C0C) << 6;
}

static void __attribute__((__noipa__))  __attribute__((optimize("O0"))) footask(void)
{
  while (1) {
    block_current_task();
  };
}


static void __attribute__((__noipa__))  __attribute__((optimize("O0"))) stat(void)
{
  while (1) {
    block_current_task();
    update_memory_statistic();
    volatile TransferInfo_t t = {.length = sizeof(MemoryStatistic_t), .start_adress = &mstat};
    uprint((unsigned int*) &t);
    SV_YIELD_TASK;
  };
}

int __attribute__((__noipa__))  __attribute__((optimize("O0"))) fibbonacci(int n)
{
   if(n == 0){
      return 0;
   } else if(n == 1) {
      return 1;
   } else {
      return (fibbonacci(n-1) + fibbonacci(n-2));
   }
}

void __attribute__((__noipa__))  __attribute__((optimize("O0"))) main_loop2(void)
{
  // svc(0);

  while (1)
  {
    fibbonacci(15);
    SV_YIELD_TASK;
    // fibbonacci(10);
    // float foo = powf((float) 2, (float) 4);
  }
}

void __attribute__((__noipa__))  __attribute__((optimize("O0"))) main_loop(void)
{

  while (1)
  {
    __builtin_unreachable();
  }
}

int __attribute__((optimize("O0"))) main_init(void)
{
  GpioObject_t *t = (GpioObject_t*) allocate(sizeof(GpioObject_t));
  init_scheduler();

  kernel_pids.idle_task = create_task(&idle_runner, 0); // pid0
  kernel_pids.transfer_handler = create_task(&transfer_handler, 0); // pid1
  kernel_pids.statistic_manager = create_task(&stat, 0); // pid2
  init_isr();
  init_uart(t);
  run_scheduler();
  
  __asm__("ldr r0, =main_loop\n"
          "mov pc,r0");
  return 0;
}



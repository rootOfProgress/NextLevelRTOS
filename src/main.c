#include "uart.h"
#include "exception.h"
#include "process/scheduler.h"
#include "process/task.h"
#include "types.h"
#include "memory.h"

void enable_exti0_cpu_irq()
{
  WRITE_REGISTER(0xE000E100, 0x1 << 6);
}

void enable_usart_cpu_irq()
{
  WRITE_REGISTER(0xE000E104, 0x1 << 5);
}

void setup_nvic_controller()
{
  // mask exti0 on line 6
  WRITE_REGISTER(0x40013C0C, 0x1 << 6);

  // line 6 triggers on rising edge
  WRITE_REGISTER(0x40013C08, 0x1 << 6);

  // line 6 triggers on falling edge
  WRITE_REGISTER(0x40013C0C, 0x1 << 6);
}

void enable_device_interrupts()
{
  WRITE_REGISTER(0xFFFC8040, READ_REGISTER(0xFFFC8040) | (0x3 << 8));;
}

void idle()
{
  __trap(YIELD_TASK, 0);
  // while (1);
}

void user_loop(void)
{
  __trap(YIELD_TASK, 0);
  // while (1) {};
}

void main_loop(void)
{
  while (1)
  {
    // __trap(YIELD_TASK, 0);

  }
}

int main_init(void)
{
  init_scheduler();
  create_task(&idle);
  create_task(&user_loop);
  run_scheduler();
  // create_task(&user_loop);
  // create_task(&enable_device_interrupts);
  // enable_exti0_cpu_irq();
  // enable_usart_cpu_irq();
  // setup_nvic_controller();
  // init_uart();


  // run_init_process();
  __asm__("ldr r0, =main_loop\n"
          "mov pc,r0");
  // idle();
  return 0;
}



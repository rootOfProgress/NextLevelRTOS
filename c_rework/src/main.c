// #include "uart.h"
#include "exception.h"
#include "process/scheduler.h"
#include "process/task.h"
#include "types.h"
#include "uart.h"
#include "memory.h"
#include "test.h"
#define EnablePrivilegedMode() __asm("SVC #0xF")
// void enable_exti0_cpu_irq()
// {
//   WRITE_REGISTER(0xE000E100, 0x1 << 6);
// }

void enable_usart_cpu_irq()
{
  *((unsigned int*) 0xE000E104) = *((unsigned int*) 0xE000E104) | 0x1 << 5;
}

void setup_nvic_controller()
{
  // mask exti0 on line 6
  *((unsigned int*) 0x40013C0C) = *((unsigned int*) 0x40013C0C) << 6;

  // line 6 triggers on rising edge
  *((unsigned int*) 0x40013C08) = *((unsigned int*) 0x40013C08) << 6;

  // line 6 triggers on falling edge
  *((unsigned int*) 0x40013C0C) = *((unsigned int*) 0x40013C0C) << 6;
}

// void enable_device_interrupts()
// {
//   WRITE_REGISTER(0xFFFC8040, READ_REGISTER(0xFFFC8040) | (0x3 << 8));;
// }

void idle()
{
  // __trap(YIELD_TASK, 0);
  // while (1) {
    // svc(1);
  // };
}

static void __attribute__((__noipa__)) __attribute__((optimize("O2"))) stat(void)
{
  // __trap(YIELD_TASK, 0);
  while (1) {
    update_statistic();
    svc(1);
  };
}

static void __attribute__((__noipa__)) __attribute__((optimize("O2"))) hans(void)
{
  // __trap(YIELD_TASK, 0);
  while (1) {
    svc(1);
  };
}

static void __attribute__((__noipa__)) __attribute__((optimize("O2"))) fritz(void)
{
  // __trap(YIELD_TASK, 0);
  while (1) {
    svc(1);
  };
}

int __attribute__((__noipa__))  __attribute__((optimize("O0"))) dofoo(int n)
{
  while (1)
  {
    /* code */
  }
  
  // return dofoo(n + 1);
}

static void __attribute__((__noipa__))  __attribute__((optimize("O0"))) wurst(void)
{
  // __trap(YIELD_TASK, 0);
  while (1) {
    // create_task(&idle);
    svc(1);
  };
}

void __attribute__((__noipa__))  __attribute__((optimize("O0"))) main_loop(void)
{
  // svc(0);
  while (1)
  {
    // __trap(YIELD_TASK, 0);

  }
}

#ifdef FOOSELF_CHECK
void do_selfcheck_main()
{
  queue_selfcheck();
  do_selfcheck_task();
  do_selfcheck_scheduler();
  do_selfcheck_svc();
}
#endif

int main_init(void)
{
  #ifdef SELF_CHECK
    // do_selfcheck_main();
    // do_selfcheck_svc();
  #endif
  init_scheduler();
  create_task(&hans, 0);
  create_task(&stat, 0);
  create_task(&wurst, 0);
  init_uart();
  enable_usart_cpu_irq();
  run_scheduler();
  
  __asm__("ldr r0, =main_loop\n"
          "mov pc,r0");
  return 0;
}



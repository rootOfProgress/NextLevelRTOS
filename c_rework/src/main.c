// #include "uart.h"
#include "exception.h"
#include "process/scheduler.h"
#include "process/task.h"
#include "types.h"
#include "memory.h"
#include "test.h"
#define EnablePrivilegedMode() __asm("SVC #0xF")
// void enable_exti0_cpu_irq()
// {
//   WRITE_REGISTER(0xE000E100, 0x1 << 6);
// }

// void enable_usart_cpu_irq()
// {
//   WRITE_REGISTER(0xE000E104, 0x1 << 5);
// }

// void setup_nvic_controller()
// {
//   // mask exti0 on line 6
//   WRITE_REGISTER(0x40013C0C, 0x1 << 6);

//   // line 6 triggers on rising edge
//   WRITE_REGISTER(0x40013C08, 0x1 << 6);

//   // line 6 triggers on falling edge
//   WRITE_REGISTER(0x40013C0C, 0x1 << 6);
// }

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
    dofoo(0);
    // create_task(&idle);
    // svc(1);
  };
}

void main_loop(void)
{
  // svc(0);
  while (1)
  {
    // __trap(YIELD_TASK, 0);

  }
}

#ifdef SELF_CHECK
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
 // __asm__ volatile("bkpt");
  #ifdef SELF_CHECK
    do_selfcheck_main();
    do_selfcheck_svc();
  #endif
  // __asm (
  //   "mov r0, #1\n"
  //   "msr control,r0\n"
  // );
  //  unsigned int volatile ctrl;
  // __asm__("mrs %0, control" : "=r"(ctrl));
  init_scheduler();
  // create_task(&idle);
  // create_task(&idle);
  // create_task(&idle);
  // create_task(&idle);  
  // create_task(&idle);
  // create_task(&idle);
  // create_task(&idle);
  // create_task(&idle);
  // create_task(&idle);
  // // create_task(&idle);
  // // create_task(&idle);
  // create_task(&idle);  
  // create_task(&idle);
  // create_task(&idle);
  // create_task(&idle);
  // create_task(&idle);
  // create_task(&hans);
  // create_task(&fritz);
  // create_task(&idle);
  // create_task(&idle);
  // create_task(&idle);
  create_task(&wurst);
  run_scheduler();
  // create_task(&user_loop);
  // create_task(&enable_device_interrupts);
  // enable_exti0_cpu_irq();
  // enable_usart_cpu_irq();
  // setup_nvic_controller();
  // init_uart();
  // EnablePrivilegedMode();

  // run_init_process();
  main_loop();
  // __asm__("ldr r0, =main_loop\n"
  //         "mov pc,r0");
  // idle();
  return 0;
}



// #include "uart.h"
#include "exception.h"
#include "process/scheduler.h"
#include "process/task.h"
#include "devices/gpio.h"
#include "types.h"
#include "devices/uart.h"
#include "memory.h"
#include "test.h"
#define EnablePrivilegedMode() __asm("SVC #0xF")


// void enable_exti0_cpu_irq()
// {
//   WRITE_REGISTER(0xE000E100, 0x1 << 6);
// }

void setup_nvic_controller()
{
  // mask exti0 on line 6
  *((unsigned int*) 0x40013C0C) = *((unsigned int*) 0x40013C0C) << 6;

  // line 6 triggers on rising edge
  *((unsigned int*) 0x40013C08) = *((unsigned int*) 0x40013C08) << 6;

  // line 6 triggers on falling edge
  *((unsigned int*) 0x40013C0C) = *((unsigned int*) 0x40013C0C) << 6;
}


static void __attribute__((__noipa__)) stat(void)
{
  while (1) {
    update_statistic();
    SV_YIELD_TASK;
  };
}

static void __attribute__((__noipa__)) hans(void)
{
  while (1) {
    SV_YIELD_TASK;
  };
}

static void __attribute__((__noipa__)) wurst(void)
{
  while (1) {
    SV_YIELD_TASK;
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
  print("hello\n\r",7);
  // GpioActions_t *t = (GpioActions_t*) allocate(sizeof(GpioActions_t));
  // t->gpio_object = (GpioObject_t*) allocate(sizeof(GpioObject_t));
  
  // t->gpio_object->number = 3;
  // t->gpio_object->port = "A";
  // init_gpio(t);
  // toggle_output_pin(t);



  init_scheduler();
  create_task(&hans, 0);
  create_task(&stat, 0);
  create_task(&wurst, 0);
  init_isr();
  init_uart();
  run_scheduler();

  
  __asm__("ldr r0, =main_loop\n"
          "mov pc,r0");
  return 0;
}



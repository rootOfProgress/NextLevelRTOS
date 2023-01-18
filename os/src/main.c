// #include "uart.h"
#include "exception.h"
#include "process/scheduler.h"
#include "process/task.h"
#include "devices/gpio.h"
#include "types.h"
#include "devices/uart.h"
#include "memory.h"
#include "test.h"
#include "drohne/rpm.h"
#define EnablePrivilegedMode() __asm("SVC #0xF")


// void enable_exti0_cpu_irq()
// {
//   WRITE_REGISTER(0xE000E100, 0x1 << 6);
// }
unsigned int glob;
unsigned int mutex;
extern void lock_mutex(void * mutex);
extern void unlock_mutex(void * mutex);

void setup_nvic_controller()
{
  // mask exti0 on line 6
  *((unsigned int*) 0x40013C0C) = *((unsigned int*) 0x40013C0C) << 6;

  // line 6 triggers on rising edge
  *((unsigned int*) 0x40013C08) = *((unsigned int*) 0x40013C08) << 6;

  // line 6 triggers on falling edge
  *((unsigned int*) 0x40013C0C) = *((unsigned int*) 0x40013C0C) << 6;
}

static void __attribute__((__noipa__))  __attribute__((optimize("O0"))) drohne_rpm(void)
{
  init_rpm_timer();
  block_current_task();
  SV_YIELD_TASK;

  while (1) {
    block_current_task();
    do_measurement();
    volatile TransferInfo_t t_rpm = {.length = sizeof(Rpm_t), .start_adress = &rpm_results};
    uprint((unsigned int*) &t_rpm, RPM);
  };
}

static void __attribute__((__noipa__))  __attribute__((optimize("O0"))) stat(void)
{
  while (1) {
    block_current_task();
    update_memory_statistic();
    volatile TransferInfo_t t = {.length = sizeof(MemoryStatistic_t), .start_adress = &mstat};
    uprint((unsigned int*) &t, STATISTIC);
  };
}

static void __attribute__((__noipa__)) __attribute__((optimize("O0"))) idle(void)
{
  while (1) {
    SV_YIELD_TASK;
  };
}

static void __attribute__((__noipa__)) __attribute__((optimize("O0"))) thread1(void)
{
  while (1) {
    // lock_mutex((void*) &mutex);
    if (++glob >= 1000)
      return;
  };
}

static void __attribute__((__noipa__)) __attribute__((optimize("O0"))) thread2(void)
{
  while (1) {
    // lock_mutex((void*) &mutex);
    if (++glob >= 1000)
      return;
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
  mutex=0;
  glob=0;
  GpioObject_t *t = (GpioObject_t*) allocate(sizeof(GpioObject_t));
  init_scheduler();
  create_task(&transfer_handler, 0); // pid0
  create_task(&stat, 0); // pid1
  create_task(&idle, 0); // pid2
  // create_task(&drohne_rpm, 0); // pid3
  create_task(&thread1, 0); // pid2
  create_task(&thread2, 0); // pid3
  init_isr();
  init_uart(t);
  run_scheduler();
  
  __asm__("ldr r0, =main_loop\n"
          "mov pc,r0");
  return 0;
}



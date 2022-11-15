#include <stdint.h>
#include "../include/uart.h"
#include "../include/exception.h"
#include "../include/process/scheduler.h"
#include "../include/types.h"
#include "../include/device.h"
#include "memory.h"

#include <stddef.h>


frame_t *task0 = (frame_t*) 0x20001000;
int addr1 = 0x20002345;
volatile static void (*fn)();

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
  while (1);
}

void user_loop(void)
{
  opcode = 3;
  __asm__("mov r0, 0x4AB\n"
          "mov r1, 0x4BC\n"
          "mov r2, 0x4CD\n"
          "mov r3, 0x4EF\n"
          "mov r4, 0x411\n"
          "mov r10, 0x488\n"
          "mov r11, 0x499\n");
  __asm__("svc 0");
  while (1) {};
}

void main_loop(void)
{
  // fn = user_loop;
  // volatile frame_t task0;
  *(frame_t*) task0 = (frame_t)
  {
    .0
  };
  task0->r4 = 0x1A;
  task0->r11 = 0x1F;
  task0->r0 = 0x2A;
  task0->r3 = 0x2F;

  task0->pc = (uint32_t) &user_loop;
  task0->xPsr = (uint32_t) 0x21000000;

  __asm__("mov r0, 0xAB\n"
          "mov r1, 0xBC\n"
          "mov r2, 0xCD\n"
          "mov r3, 0xEF\n"
          "mov r4, 0x11\n"
          "mov r10, 0x88\n"
          "mov r11, 0x99\n");

  opcode = 1;
  __asm__("svc 0");

  opcode = 2;
  arg = (void*) task0;
  __asm__("svc 0");
  // trap(1);
  while (1)
  {

  }
}

int main_init(void)
{
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



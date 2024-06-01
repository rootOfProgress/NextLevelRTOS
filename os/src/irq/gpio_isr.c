#include "irq/gpio_isr.h"
#include "memory.h"
#include "lang.h"
#include "hw/cpu.h"
#include "core/exception.h"

GpioIsrConfig gpio_config_exti;

void enable_cpu_interrupt_exti(void)
{
  enable_nvic_interrupt(gpio_config_exti.exti_pos_vector_table);
}

// currently supported: pin 0 - 4, every port.  general: Pin An-Hn is EXTI_n (n = 0 - 15) p203 sec 10.2
int link_exti_src(void (*isr_callback)(), GpioObject_t *gpio_obj)
{
  // page 203 refman: WWDG is at position zero (first nv ir), so from that
  // 6 slots higher EXTI0 begines
  int exti_offset = 6;
  // not supported
  if (gpio_obj->pin <= 4)
  {
    exti_offset = 6;
    gpio_config_exti.exti_pos_vector_table = gpio_obj->pin + exti_offset;
    gpio_config_exti.isr_handler[gpio_obj->pin] = isr_callback;

  }
  else if (gpio_obj->pin > 4 && gpio_obj->pin < 10)
  {
    // offset for pin 5 - 9 is 23
    gpio_config_exti.exti_pos_vector_table = 23;
    gpio_config_exti.isr_handler[Exti9_5] = isr_callback;
  }

  init_gpio(gpio_obj);
  set_moder(gpio_obj, InputMode);
  // @todo: if pin = 6, then  exti_pos_vector_table gets overwritten!
  execute_priviledged((unsigned int) &enable_cpu_interrupt_exti);

  return 1;
}

void NO_OPT empty_isr(void)
{
  return;
}

// just to prevent hardfault on empty isr access
void init_exti_isr_handling(void)
{
  for (int i = 0; i < NumberOfSupportedExtiLines; i++)
  {
    gpio_config_exti.isr_handler[i] = empty_isr;
  }
}

void NO_OPT INTERRUPT exti0_isr_handler()
{
  gpio_config_exti.isr_handler[Exti0]();
}

void NO_OPT INTERRUPT exti1_isr_handler()
{
  gpio_config_exti.isr_handler[Exti1]();
}

void NO_OPT INTERRUPT exti2_isr_handler()
{
  gpio_config_exti.isr_handler[Exti2]();
}

void NO_OPT INTERRUPT exti3_isr_handler()
{
  gpio_config_exti.isr_handler[Exti3]();
}

void NO_OPT INTERRUPT exti4_isr_handler()
{
  gpio_config_exti.isr_handler[Exti4]();
}

void NO_OPT INTERRUPT exti9_5_isr_handler()
{
  gpio_config_exti.isr_handler[Exti9_5]();
}

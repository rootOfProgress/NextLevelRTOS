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
#include "gpio/gpio.h"
#include "i2c.h"
#include "tim2_5.h"
#include "uart.h"

#include "test.h"
#include "math.h"
#include "runtime.h"

unsigned int __errno = 0;

// @todo: call gpio / uart driver directly
void setup_devices(void)
{
  GpioObject_t *t = (GpioObject_t*) allocate(sizeof(GpioObject_t));
  init_uart(t);
  init_isr();
}


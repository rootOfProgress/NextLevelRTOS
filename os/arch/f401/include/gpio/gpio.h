#ifndef GPIO_H
#define GPIO_H
#define READ_REGISTER(addr)     (*(volatile unsigned int *) (addr))
#define WRITE_REGISTER(addr, val) ((*(volatile unsigned int *) (addr)) = (unsigned int) (val))

#include "gpio_types.h"

// page 38
enum
{
  GpioABase = 0x40020000,
  GpioBBase = 0x40020400,
  GpioCBase = 0x40020800,
  GpioDBase = 0x40020C00,
};

#define INPUT 0b00
#define GENERALPURPOSEOUTPUT 0b01
#define ALTERNATE 0b10
#define ANALOG 0b11

void set_otyper(GpioObject_t*, OutputTypes_t);
void set_moder(GpioObject_t*, ModerTypes_t);
void into_af(GpioObject_t*, unsigned int);
void toggle_output_pin(GpioObject_t*);
void init_gpio(GpioObject_t*);
void set_pupdr(GpioObject_t*, PullTypes_t);
void set_speed(GpioObject_t*, SpeedModes_t);
void gpio_set_registermap(GpioObject_t* t);

static inline __attribute__((always_inline)) void set_pin_on(GpioObject_t* gpio)
{
  WRITE_REGISTER((unsigned int*) & (gpio->gpio_regs->odr), READ_REGISTER(&(gpio->gpio_regs->odr)) | (1 << gpio->pin));
}

static inline __attribute__((always_inline)) void set_pin_off(GpioObject_t* gpio)
{
  WRITE_REGISTER((unsigned int*) & (gpio->gpio_regs->odr), READ_REGISTER(&(gpio->gpio_regs->odr)) & ~(1 << gpio->pin));
}

static inline __attribute__((always_inline)) void set_pin_bulk(GpioObject_t* gpio, unsigned int mask)
{
  WRITE_REGISTER((unsigned int*) & (gpio->gpio_regs->odr), mask);
}

static inline __attribute__((always_inline)) unsigned int read_pin(GpioObject_t* gpio)
{
  return READ_REGISTER(&(gpio->gpio_regs->idr)) & (1 << gpio->pin);
}

#endif
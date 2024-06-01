#ifndef GPIO_H
#define GPIO_H
#include "baseadresses.h"

#define GPIO_A_BASE 0x48000000
#define GPIO_B_BASE 0x48000400
#define GPIO_C_BASE 0x48000800
#define GPIO_D_BASE 0x48001000

#define INPUT 0b00
#define GENERALPURPOSEOUTPUT 0b01
#define ALTERNATE 0b10
#define ANALOG 0b11

typedef struct Gpio
{
  unsigned int* moder;
  unsigned int* otyper;
  unsigned int* ospeedr;
  unsigned int* pupdr;
  unsigned int* idr;
  unsigned int* odr;
  unsigned int* bsrr;
  unsigned int* lckr;
  unsigned int* afrl;
  unsigned int* afrh;
  unsigned int* brr;
} GpioRegisters_t;

typedef enum OutputTypes
{
  PushPull,
  OpenDrain,
} OutputTypes_t;

typedef enum SpeedModes
{
  Low,
  Medium,
  Fast,
  High,
} SpeedModes_t;

typedef enum PullTypes
{
  Nothing,
  PullUp,
  PullDown,
} PullTypes_t;

typedef enum ModerTypes
{
  InputMode,
  GeneralPurposeOutputMode,
  AlternateFunctionMode,
  AnalogMode,
} ModerTypes_t;

typedef enum OutputState
{
  On,
  Off,
} OutputState_t;

typedef struct GpioObject
{
  char port;
  unsigned int pin;
  unsigned int *base_adress;
} GpioObject_t;

typedef struct GpioActions
{
  GpioObject_t *gpio_object;
} GpioActions_t;

void set_otyper(GpioObject_t*, OutputTypes_t);
void set_moder(GpioObject_t*, ModerTypes_t);
GpioRegisters_t* get_registers(GpioObject_t*);
void into_af(GpioObject_t*, unsigned int);
void toggle_output_pin(GpioObject_t*);
void init_gpio(GpioObject_t*);

static inline __attribute__((always_inline)) void set_pin_on(GpioObject_t* gpio)
{
}

static inline __attribute__((always_inline)) unsigned int read_pin(GpioObject_t* gpio)
{
}

static inline __attribute__((always_inline)) void set_pin_off(GpioObject_t* gpio)
{
}

#endif
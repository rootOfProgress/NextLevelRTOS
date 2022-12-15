#ifndef GPIO_H
#define GPIO_H
#include "baseadresses.h"

const unsigned int* GPIO_A_BASE = (unsigned int*) 0x48000000;

typedef struct Gpio {
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
} Gpio_t;

typedef enum OutputTypes {
    PushPull,
    OpenDrain,
} OutputTypes_t;

typedef enum SpeedModes {
    Low,
    Medium,
    Fast,
    High,
} SpeedModes_t;

typedef enum PullTypes {
    Nothing,
    PullUp,
    PullDown,
} PullTypes_t;

typedef enum ModerTypes {
    InputMode,
    GeneralPurposeOutputMode,
    AlternateFunctionMode,
    AnalogMode,
} ModerTypes_t;

typedef enum OutputState {
    On,
    Off,
} OutputState_t;

typedef struct GpioObject {
    char port;
    unsigned int number;
    unsigned int *base_adress;
} GpioObject_t;

typedef struct GpioActions {
    void (*toggle)(GpioObject_t*);
    GpioObject_t *gpio_object;
} GpioActions_t;

void toggle_output_pin(GpioObject_t*);
void init_gpio(GpioActions_t*);

#endif
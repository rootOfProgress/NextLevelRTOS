#ifndef GPIO_TYPES_H
#define GPIO_TYPES_H

typedef struct Gpio {
    unsigned int moder;
    unsigned int otyper;
    unsigned int ospeedr; //0x08
    unsigned int pupdr;
    unsigned int idr; //0x10
    unsigned int odr;
    unsigned int bsrr; //0x18
    unsigned int lckr; //0x1c
    unsigned int afrl; //0x20
    unsigned int afrh; //0x24
} GpioRegisters_t;

typedef enum SpeedModes {
    Low,
    Medium,
    Fast,
    High,
} SpeedModes_t;

typedef enum PullTypes {
    Nothing = 0,
    PullUp = 1,
    PullDown = 2,
} PullTypes_t;

typedef enum OutputTypes {
    PushPull,
    OpenDrain,
} OutputTypes_t;

typedef enum ModerTypes {
    InputMode,
    GeneralPurposeOutputMode,
    AlternateFunctionMode,
    AnalogMode,
} ModerTypes_t;

typedef struct GpioObject {
    char port;
    unsigned int pin;
    unsigned int *base_adress;
    GpioRegisters_t *gpio_regs;
} GpioObject_t;

typedef struct GpioActions {
    GpioObject_t *gpio_object;
} GpioActions_t;

#endif
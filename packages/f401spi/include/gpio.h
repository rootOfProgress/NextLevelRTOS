#ifndef GPIO_H
#define GPIO_H

// page 38
#define GPIO_A_BASE 0x40020000
#define GPIO_B_BASE 0x40020400
#define GPIO_C_BASE 0x40020800
#define GPIO_D_BASE 0x40020C00

#define INPUT 0b00
#define GENERALPURPOSEOUTPUT 0b01
#define ALTERNATE 0b10
#define ANALOG 0b11

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


typedef struct GpioObject {
    char port;
    unsigned int pin;
    unsigned int *base_adress;
} GpioObject_t;

typedef struct GpioActions {
    GpioObject_t *gpio_object;
} GpioActions_t;

GpioRegisters_t* get_registers(GpioObject_t*);

void set_otyper(GpioObject_t*, OutputTypes_t);
void set_moder(GpioObject_t*, ModerTypes_t);
void into_af(GpioObject_t*, unsigned int);
void toggle_output_pin(GpioObject_t*);
void set_pin_on(GpioObject_t*);
void set_pin_off(GpioObject_t*);
void init_gpio(GpioObject_t*);

#endif
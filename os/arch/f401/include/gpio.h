#ifndef GPIO_H
#define GPIO_H
#define READ_REGISTER(addr)     (*(volatile unsigned int *) (addr))
#define WRITE_REGISTER(addr, val) ((*(volatile unsigned int *) (addr)) = (unsigned int) (val))

// page 38
enum {
    GpioABase = 0x40020000,
    GpioBBase = 0x40020400,
    GpioCBase = 0x40020800,
    GpioDBase = 0x40020C00,
};

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
    WRITE_REGISTER((unsigned int*) &(gpio->gpio_regs->odr), READ_REGISTER(&(gpio->gpio_regs->odr)) | (1 << gpio->pin));
}

static inline __attribute__((always_inline)) void set_pin_off(GpioObject_t* gpio) 
{
    WRITE_REGISTER((unsigned int*) &(gpio->gpio_regs->odr), READ_REGISTER(&(gpio->gpio_regs->odr)) & ~(1 << gpio->pin));
}

static inline __attribute__((always_inline)) void set_pin_bulk(GpioObject_t* gpio, unsigned int mask) 
{
    WRITE_REGISTER((unsigned int*) &(gpio->gpio_regs->odr), mask);
}

static inline __attribute__((always_inline)) unsigned int read_pin(GpioObject_t* gpio) 
{
    return READ_REGISTER(&(gpio->gpio_regs->idr)) & (1 << gpio->pin);
}

#endif
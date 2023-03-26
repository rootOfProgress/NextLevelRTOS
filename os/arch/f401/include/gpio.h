#ifndef GPIO_H
#define GPIO_H
#define READ_REGISTER(addr)     (*(volatile unsigned int *) (addr))
#define WRITE_REGISTER(addr, val) ((*(volatile unsigned int *) (addr)) = (unsigned int) (val))

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

typedef enum SpeedModes {
    Low,
    Medium,
    Fast,
    High,
} SpeedModes_t;

typedef enum PullTypes {
    Nothing,
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

static inline __attribute__((always_inline)) GpioRegisters_t* get_registers(GpioObject_t* t)
{
    switch (t->port)
    {
    case 'A':
        return (GpioRegisters_t*) ((unsigned int*) GPIO_A_BASE);
    case 'B':
        return (GpioRegisters_t*) ((unsigned int*) GPIO_B_BASE);
    case 'C':
        return (GpioRegisters_t*) ((unsigned int*) GPIO_C_BASE);
    case 'D':
        return (GpioRegisters_t*) ((unsigned int*) GPIO_D_BASE);
    default:
        return (GpioRegisters_t*) ((unsigned int*) GPIO_A_BASE);
    }
}

static inline __attribute__((always_inline)) void set_pin_on(GpioObject_t* gpio) 
{
    GpioRegisters_t* gpio_regs = get_registers(gpio);
    WRITE_REGISTER((unsigned int*) &gpio_regs->odr, READ_REGISTER(&gpio_regs->odr) | (1 << gpio->pin));
}

static inline __attribute__((always_inline)) unsigned int read_pin(GpioObject_t* gpio) 
{
    GpioRegisters_t* gpio_regs = get_registers(gpio);
    return READ_REGISTER(&gpio_regs->idr) & (1 << gpio->pin);
}

static inline __attribute__((always_inline)) void set_pin_off(GpioObject_t* gpio) 
{
    GpioRegisters_t* gpio_regs = get_registers(gpio);
    WRITE_REGISTER((unsigned int*) &gpio_regs->odr, READ_REGISTER(&gpio_regs->odr) & ~(1 << gpio->pin));
}

#endif
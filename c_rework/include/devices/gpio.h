#ifndef GPIO_H
#define GPIO_H


typedef struct GpioObject {
    char port;
    unsigned int number;
    unsigned int *base_adress;
} GpioObject_t;

typedef struct GpioActions {
    void (*toggle)();
    GpioObject_t* gpio_object;
} GpioActions_t;

extern void init_gpio(GpioActions_t*);
extern void toggle_output_pin(GpioObject_t*);
#endif
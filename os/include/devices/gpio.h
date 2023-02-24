#ifndef GPIO_H
#define GPIO_H


typedef struct GpioObject {
    char port;
    unsigned int pin;
    unsigned int *base_adress;
} GpioObject_t;

typedef struct GpioActions {
    void (*toggle)();
    GpioObject_t* gpio_object;
} GpioActions_t;

extern void init_gpio(GpioActions_t*);
extern void toggle_output_pin(GpioObject_t*);
extern void set_pin_on(GpioObject_t*); 
extern void set_pin_off(GpioObject_t*); 
extern unsigned int read_pin(GpioObject_t*);


#endif
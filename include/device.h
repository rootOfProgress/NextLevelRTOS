#ifndef DEVICE_H
#define DEVICE_H

enum dev_type {
    GPIO = 0,
    I2C,
};

typedef struct {
    char port;
    short number;
} gpio_ident;

typedef struct {
    void (*turn_on)(gpio_ident *);
} gpio_op;

typedef struct {
    enum dev_type dt;
    void* ident;
} dev_type_args;

void set_on(gpio_ident *);
#endif
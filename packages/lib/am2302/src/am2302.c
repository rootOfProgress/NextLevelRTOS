#include "am2302.h"

GpioObject_t gpio;
Am2302Readings_t readings;

void init_am2302_gpio(GpioObject_t* obj)
{
    init_gpio(obj);

    set_moder(obj, GeneralPurposeOutputMode);
    set_otyper(obj, PushPull);
    set_speed(obj, High);
    set_pin_off(obj);
}

void am2302_init_peripherials(unsigned int gpio_pin, char gpio_port) 
{
    gpio.pin = gpio_pin;
    gpio.port = gpio_port;
    init_am2302_gpio(&gpio);

    set_pin_on(&gpio);

    // resolution: 
    timer_init(timerNumber, 1, (unsigned int[4]) {0,0,0,0}, 1);    
}


void am2302_do_measurement(Am2302Readings_t* result)
{
    am2302_send_host_init();

    am2302_wait_for_sensor();
    am2302_record(result);
    // @todo: Don't return a local pointer!!

    // return ar;
}
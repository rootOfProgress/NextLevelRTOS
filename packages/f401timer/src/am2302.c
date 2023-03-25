#include "am2302.h"

GpioObject_t gpio;

void init_am2302_gpio(GpioObject_t* obj)
{
    init_gpio(obj);

    set_moder(obj, GeneralPurposeOutputMode);
    set_otyper(obj, PushPull);
    set_speed(obj, High);
    set_pin_off(obj);
}


void am2302_do_measurement(void)
{
    gpio.pin = 0;
    gpio.port = 'A';
    init_am2302_gpio(&gpio);
    set_pin_on(&gpio);
    // resolution: 
    timer_init(timerNumber, 1, (char[4]) {0,0,0,0}, 1);

    am2302_send_host_init();
    am2302_wait_for_sensor();
    am2302_record();
}
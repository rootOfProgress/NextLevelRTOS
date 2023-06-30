#ifndef AM2302_H
#define AM2302_H

#include "tim2_5.h"
#include "gpio.h"

typedef struct am2302_readings {
    unsigned short rh;
    unsigned short temp;
    char crc;
    char is_valid;
} Am2302Readings_t;

// all usec
typedef enum hostTimings {
    HostPullsLow = 1000,
    HostPullsUp_Min = 20,
    HostPullsUp_Max = 40,
} HostTimings_t;

// all usec
typedef enum sensorTimings {
    SensorPullsLow = 80,
    SensorPullsUp = 80,
    StartTransmitSignal = 50,
    SensorOutputZero = 30,
} SensorTimings_t;

extern Am2302Readings_t readings;
extern GpioObject_t gpio;
enum { timerNumber = 2 };

static inline __attribute__((always_inline)) void am2302_send_host_init(void) 
{
    set_pin_off(&gpio);
    timer_start(timerNumber);
    while (timer_read_counter(timerNumber) < HostPullsLow) {}
    timer_stop(timerNumber);

    timer_flush_counter(timerNumber);


    timer_start(timerNumber);
    set_pin_on(&gpio);
    while (timer_read_counter(timerNumber) < SensorOutputZero) {}
    set_pin_off(&gpio);
    timer_stop(timerNumber);
    timer_flush_counter(timerNumber);
}

static inline __attribute__((always_inline)) void am2302_wait_for_sensor(void)
{
    set_moder(&gpio, InputMode);

    while (!read_pin(&gpio)) {}
    while (read_pin(&gpio)) {}    
}

static inline __attribute__((always_inline)) Am2302Readings_t* am2302_record(void)
{
    unsigned int prev_state = 0;
    unsigned int elapsed;
    unsigned long long result = 0ULL;
    // rh : 39 - 24 | temp: 23 - 8 | crc : 7 - 0
    unsigned long long iteration = 39;

    unsigned int t_now; 
    timer_start(timerNumber);
    
    while ((elapsed = timer_read_counter(timerNumber)) < 10000) // 10 ms
    {
        // rising edge
        if (prev_state < read_pin(&gpio))
        {
            t_now = timer_read_counter(timerNumber);
            prev_state = read_pin(&gpio);
        } 
        // falling edge
        else if (prev_state > read_pin(&gpio))
        {
            if (!((timer_read_counter(timerNumber) - t_now) < SensorOutputZero))
            {
                result |= (1ULL << iteration);
            }
            iteration--;
            prev_state = read_pin(&gpio);
        }
    }
    timer_stop(timerNumber);

    readings.rh = (unsigned short) (result >> 24);
    readings.temp = (unsigned short) (result >> 8) & 0xFFFF;
    readings.crc = (char) result & 0xFF;
    readings.is_valid = 0;

    if ((char)((readings.rh & 0xFF) + (readings.rh >> 8) + (readings.temp & 0xFF) + (readings.temp >> 8)) == readings.crc)
        readings.is_valid = 1;

    return &readings;
}

void am2302_init_peripherials(unsigned int, char); 
Am2302Readings_t* am2302_do_measurement(void);

#endif
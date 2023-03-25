#ifndef AM2302_H
#define AM2302_H

#include "tim2_5.h"
#include "gpio.h"

typedef struct readings {
    unsigned short rh;
    unsigned short temp;
    char crc;
    char is_valid;
} readings_t;

// all usec
typedef enum hostTimings {
    HostPullsLow = 1000,
    HostPullsUp_Min = 20,
    HostPullsUp_Max = 40,
} hostTimings_t;

// all usec
typedef enum sensorTimings {
    SensorPullsLow = 80,
    SensorPullsUp = 80,
    StartTransmitSignal = 50,
    SensorOutputZeroMin = 26,
    SensorOutputZeroMax = 28,
    SensorOutputOne = 70,
} sensorTimings_t;

extern GpioObject_t gpio;
enum { timerNumber = 3 };


static inline __attribute__((always_inline)) void am2302_send_host_init(void) 
{

    set_pin_off(&gpio);
    // asm("bkpt");
    timer_start(3);
    while (read_counter(timerNumber) < 1000) {}
    timer_stop(3);

    flush_counter(timerNumber);
    // asm("bkpt");


    timer_start(3);
    set_pin_on(&gpio);
    while (read_counter(timerNumber) < 30) {}
    set_pin_off(&gpio);
    timer_stop(3);
    flush_counter(timerNumber);

}

static inline __attribute__((always_inline)) void am2302_wait_for_sensor(void)
{
    set_moder(&gpio, InputMode);

    unsigned int prev_state;// = read_pin(&gpio);
    while (!read_pin(&gpio)) {}
    while (read_pin(&gpio)) {}

    unsigned int elapsed;
    char zeros = 0;
    char ones = 0;
    unsigned long long result = 0ULL;
    // rh : 39 - 24 | temp: 23 - 8 | crc : 7 - 0
    unsigned long long iteration = 39;

    unsigned short rh = 0;
    unsigned short t = 0;
    char crc = 0;
    char bar[40];
    for (unsigned int i = 0; i < 40; i++)
        bar[i] = 0;
    unsigned int t_now; 
    timer_start(timerNumber);
    
    prev_state = 0;//read_pin(&gpio);
    while ((elapsed = read_counter(timerNumber)) < 10000) // 10 ms
    {
        // rising edge
        if (prev_state < read_pin(&gpio))
        {
            t_now = read_counter(3);
            prev_state = read_pin(&gpio);
        } 
        // falling edge
        else if (prev_state > read_pin(&gpio))
        {
            if (!((read_counter(timerNumber) - t_now) < 30))
            {
                // if (iteration > 23)
                    // rh |= (1 << iteration);
                // else if (iteration > 7)
                    // t |= (1 << iteration);
                // else 
                    // crc |= (1 << iteration); 
                result |= (1ULL << iteration);
                // bar[iteration++] = 1;
                ones++;
            } else {
                // bar[iteration++] = 0;
                zeros++;
            }
            iteration--;
            // if ((read_counter(timerNumber) - t_now) < 30)
            // {
            //     zeros++;
            // }
            // else
            // {
            //     ones++;
            // }
            prev_state = read_pin(&gpio);
        }
    }

    readings_t readings;
    readings.rh = (unsigned short) (result >> 24);
    readings.temp = (unsigned short) (result >> 8) & 0xFFFF;
    readings.crc = (char) result & 0xFF;
    readings.is_valid = 0;
    char isok = 0;
    if ((char)((readings.rh & 0xFF) + (readings.rh >> 8) + (readings.temp & 0xFF) + (readings.temp >> 8)) == readings.crc)
        readings.is_valid = 1;

    asm("bkpt");
    timer_stop(timerNumber);
    
}
static inline __attribute__((always_inline)) void am2302_record(void);
void am2302_do_measurement(void);

#endif
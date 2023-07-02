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


void am2302_init_peripherials(unsigned int, char); 
void am2302_do_measurement(Am2302Readings_t*);
void am2302_reset_gpio(void);

static inline __attribute__((always_inline)) __attribute__((optimize("O0"))) void am2302_run(Am2302Readings_t* measurement_results) 
{
    timer_flush_counter(timerNumber);

    // am2302_record
    unsigned int prev_state = 0;
    unsigned int elapsed;
    unsigned long long result = 0ULL;
    // rh : 39 - 24 | temp: 23 - 8 | crc : 7 - 0
    unsigned long long iteration = 39;
    unsigned int total = 0;

    #ifdef DEBUG
    const int BUFFERS = 64;

    typedef enum edge {
        FALLING = 0,
        RISING = 1,
        UNDEFINED = 2,
    } edge_t;

    typedef struct measurementsDetail {
        unsigned int previous_state;
        unsigned int time;
        unsigned int current_state;
        unsigned int time_total;
        edge_t edge;
    } measurementsDetail_t; 

    measurementsDetail_t debug[BUFFERS];
    for (int i = 0; i < BUFFERS; i++)
    {
        debug[i].previous_state = 42;
        debug[i].current_state = 0;
        debug[i].time_total = 0;
        debug[i].time = 0;
        debug[i].edge = UNDEFINED;
    }
    #endif

    // am2302_send_host_init
    set_moder(&gpio, GeneralPurposeOutputMode);
    set_pin_on(&gpio);
    for (unsigned int i = 0; i < 2000; i++) {}
    
    set_pin_off(&gpio);
    timer_start(timerNumber);
    while (timer_read_counter(timerNumber) < HostPullsLow) {}
    timer_stop(timerNumber);

    timer_flush_counter(timerNumber);

    // wait for sensor response
    timer_start(timerNumber);
    set_pin_on(&gpio);
    while (timer_read_counter(timerNumber) < SensorOutputZero) {}
    set_pin_off(&gpio);
    timer_stop(timerNumber);
    timer_flush_counter(timerNumber);

    // am2302_wait_for_sensor
    set_moder(&gpio, InputMode);

    while (!read_pin(&gpio)) {}
    while (read_pin(&gpio)) {}    

    #ifdef DEBUG
    unsigned int round = 0;
    debug[round % BUFFERS].previous_state = prev_state;
    debug[round % BUFFERS].edge = UNDEFINED;
    round++;
    #endif

    unsigned int t_now; 
    timer_start(timerNumber);
    unsigned int current_state;
    
    while ((elapsed = timer_read_counter(timerNumber)) < 10000) // 10 ms
    {
        current_state = read_pin(&gpio);
        // rising edge
        if (prev_state < current_state)
        {
            t_now = timer_read_counter(timerNumber);
            
            #ifdef DEBUG
            debug[round % BUFFERS].previous_state = prev_state;
            debug[round % BUFFERS].current_state = current_state;
            debug[round % BUFFERS].time = t_now;
            debug[round % BUFFERS].edge = RISING;
            debug[round % BUFFERS].time_total = elapsed;
            round++;
            #endif
            prev_state = current_state;
        } 
        // falling edge
        else if (prev_state > current_state)
        {
            if (!((timer_read_counter(timerNumber) - t_now) < SensorOutputZero))
            {
                result |= (1ULL << iteration);
            }
            iteration--;
            #ifdef DEBUG
            debug[round % BUFFERS].previous_state = prev_state;
            debug[round % BUFFERS].current_state = current_state;
            debug[round % BUFFERS].time = t_now;
            debug[round % BUFFERS].edge = FALLING;
            debug[round % BUFFERS].time_total = elapsed;
            round++;
            #endif
            prev_state = current_state;
        }
        else
        {
            /* do nothing */
        }
    }
    timer_stop(timerNumber);

    measurement_results->rh = (unsigned short) (result >> 24);
    measurement_results->temp = (unsigned short) (result >> 8) & 0xFFFF;
    measurement_results->crc = (char) result & 0xFF;
    measurement_results->is_valid = 0;

    if ((char)((measurement_results->rh & 0xFF) + (measurement_results->rh >> 8) + (measurement_results->temp & 0xFF) + (measurement_results->temp >> 8)) == measurement_results->crc)
        measurement_results->is_valid = 1;

    set_pin_on(&gpio);
    timer_flush_counter(timerNumber);
}


#endif
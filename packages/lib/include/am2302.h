#ifndef AM2302_H
#define AM2302_H

#include "tim2_5.h"
#include "gpio/gpio.h"

// @deprecated
typedef struct am2302_readings
{
  unsigned short rh;
  unsigned short temp;
  char crc;
  char is_valid;
} Am2302Readings_t;

typedef struct
{
  unsigned short humidity;
  unsigned short temperature;
} Am2302Measurements_t;

// all usec
typedef enum hostTimings
{
  HostPullsLow = 1000,
  HostPullsUp_Min = 20,
  HostPullsUp_Max = 40,
} HostTimings_t;

// all usec
typedef enum sensorTimings
{
  SensorPullsLow = 80,
  SensorPullsUp = 80,
  StartTransmitSignal = 50,
  SensorOutputZero = 30,
} SensorTimings_t;

extern Am2302Readings_t readings;
extern GpioObject_t gpio;
enum { timerNumber = 2 };
// void append_os_core_function(unsigned int (*function_ptr)());
void am2302_init_peripherials(unsigned int, char);
void am2302_reset_gpio(void);
unsigned int am2302_do_measurement(Am2302Readings_t* measurement_results);


#endif
#include "am2302.h"
#include "globals.h"

GpioObject_t gpio;


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
}

unsigned int __attribute__((optimize("O0"))) am2302_do_measurement(Am2302Readings_t* measurement_results)
{
  __asm ("CPSID I");
  // timer_flush_counter(timerNumber);

  // am2302_record
  unsigned int prev_state = 0;
  unsigned long long result = 0ULL;
  // rh : 39 - 24 | temp: 23 - 8 | crc : 7 - 0
  unsigned long long iteration = 39;

#ifdef DEBUG
  const int BUFFERS = 64;

  typedef enum edge
  {
    FALLING = 0,
    RISING = 1,
    UNDEFINED = 2,
  } edge_t;

  typedef struct measurementsDetail
  {
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
  unsigned int tStart = osCoreFunctions[0]();
  while ((osCoreFunctions[0]() - tStart) < HostPullsLow) {}

  // wait for sensor response
  tStart = osCoreFunctions[0]();
  set_pin_on(&gpio);
  while ((osCoreFunctions[0]() - tStart) < SensorOutputZero) {}
  set_pin_off(&gpio);

  // am2302_wait_for_sensor
  set_moder(&gpio, InputMode);

  tStart = osCoreFunctions[0]();
  while (!read_pin(&gpio))
  {
    if (osCoreFunctions[0]() - tStart > 15000)
    {
      return 0;
    }
  }
  tStart = osCoreFunctions[0]();

  while (read_pin(&gpio))
  {
    if (osCoreFunctions[0]() - tStart > 15000)
    {
      return 0;
    }
  }

#ifdef DEBUG
  unsigned int round = 0;
  debug[round % BUFFERS].previous_state = prev_state;
  debug[round % BUFFERS].edge = UNDEFINED;
  round++;
#endif

  unsigned int t_now;
  unsigned int current_state;
  tStart = osCoreFunctions[0]();

  while ((osCoreFunctions[0]() - tStart) < 10000) // 10 ms
  {
    current_state = read_pin(&gpio);
    // rising edge
    if (prev_state < current_state)
    {
      t_now = osCoreFunctions[0]();

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
      if (!((osCoreFunctions[0]() - t_now) < SensorOutputZero))
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
  }

  measurement_results->rh = (unsigned short) (result >> 24);
  measurement_results->temp = (unsigned short) (result >> 8) & 0xFFFF;
  measurement_results->crc = (char) result & 0xFF;
  measurement_results->is_valid = 0;

  if ((char)((measurement_results->rh & 0xFF) +
             (measurement_results->rh >> 8) +
             (measurement_results->temp & 0xFF) +
             (measurement_results->temp >> 8)) == measurement_results->crc)
  {
    measurement_results->is_valid = 1;
  }

  set_pin_on(&gpio);
    __asm ("CPSIE I");

  return measurement_results->is_valid;
}
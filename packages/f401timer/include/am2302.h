#ifndef AM2302_H
#define AM2302_H

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

static inline __attribute__((always_inline)) void am2302_send_host_init(void);
static inline __attribute__((always_inline)) void am2302_wait_for_sensor(void);
static inline __attribute__((always_inline)) void am2302_record(void);
void am2302_do_measurement(void);

#endif
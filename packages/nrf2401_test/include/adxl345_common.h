#ifndef ADXL345_H
#define ADXL345_H

typedef struct readings {
    short x;
    short y;
    short z;
} readings_t;


enum {
    IR_WATERMARK = 1 << 1
};

char axis_dist = 6;

enum {
    NUM_OF_SAMPLES = 6
};

signed int samples[NUM_OF_SAMPLES];

enum {
    RANGE_2G = 0,
    RANGE_4G,
    RANGE_8G,
    RANGE_16G,
};

enum {
    INT_ENABLE = 0x2E,
    INT_MAP = 0x2F,
    POWER_CTL = 0x2D,
    INT_SOURCE = 0x30,
    DATA_FORMAT = 0x31,
    FIFO_CTL = 0x38,
    FIFO_STATUS = 0x39,
};

typedef struct adxl345_state {
    union 
    {
        struct 
        {
            char entries : 6, : 1, fifo_trig : 1;
        } fifo_status_bits;
        char raw;
    } fifo_status;
    
} adxl345_state_t;

// typedef union adxl345_state
// {

// } adxl345_state_t;


#endif
#ifndef POSITION_H
#define POSITION_H

extern void write(unsigned int, char *, unsigned int);
extern unsigned int read(unsigned int, unsigned int, unsigned int);

#define ADXL345 0x53
void calculate_position(void);

typedef struct readings {
    unsigned int buffer[4];
} readings_t;

extern readings_t position_readings;

#endif

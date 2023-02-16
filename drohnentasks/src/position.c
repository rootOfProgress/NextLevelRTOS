#include "position.h"

readings_t position_readings;

signed int complement(unsigned int payload, unsigned int pos, char axis)
{
    unsigned int result = 0;

    // bit 9
    signed int sign = (payload >> (pos - 1)) & ~(0xFFFE);

    // bit 8 .. 0
    signed int value = (value) & ~(0xFE00);
    // negative
    if (sign == 1)
    {
        result = (((512 - value) + 1) * -1) / 256;
        if (axis == 'y' && result == -1)
            return 0;
    }
    else
    {
        if (axis == 'y' && ((value + 1) / 256) == 1)
            return 0;
    }
    return result;
}


void calculate_position(void)
{
    char payload[2] = {0x2D, 8};
    write(ADXL345, &payload, 2);
    read(0x00, ADXL345, 1);

    position_readings.x |= read(0x32, ADXL345, 1);
    position_readings.x |= (read(0x33, ADXL345, 1) << 8);
    position_readings.y |= read(0x34, ADXL345, 1);
    position_readings.y |= (read(0x35, ADXL345, 1) << 8);
    position_readings.z |= read(0x36, ADXL345, 1);
    position_readings.z |= (read(0x37, ADXL345, 1) << 8);

    position_readings.x = complement(position_readings.x, 10, 'x');
    position_readings.y = complement(position_readings.y, 10, 'y');
    position_readings.z = complement(position_readings.z, 10, 'z');

}
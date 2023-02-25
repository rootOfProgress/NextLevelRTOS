#ifndef ADXL345_H
#define ADXL345_H


enum {
    IR_WATERMARK = 1 << 1
};



enum {
    INT_ENABLE = 0x2E,
    INT_MAP = 0x2F,
    INT_SOURCE = 0x30,
    FIFO_CTL = 0x38,
    FIFO_STATUS = 0x39,
};

#endif
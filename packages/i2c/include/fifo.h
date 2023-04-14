#ifndef FIFO_H
#define FIFO_H

enum {
    BYPASS = 0,
    FIFO,
    STREAM,
    TRIGGER
};

typedef union fifoCtl
{
    struct
    {
        char samples : 5, trigger : 1, fifo_mode : 2;
    } ctl;
    char raw;
} fifoCtl_t;

#endif
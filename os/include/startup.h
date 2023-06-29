#ifndef STARTUP_H
#define STARTUP_H

typedef struct BootFlags {
    unsigned int reinit_uart : 1, : 31;
} BootFlags_t;

#endif
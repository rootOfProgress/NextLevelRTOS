#ifndef EXCEPTION_H
#define EXCEPTION_H
#include <stdint.h>
#include "lang.h"

// extern volatile uint32_t sp_msp;
// extern volatile uint32_t sp_psp;
// extern volatile uint32_t opcode;
// extern volatile void* arg;

typedef enum {
    RUN_THREAD_MODE = 0,
    ALLOC,
    DEVICE_REQUEST,
    RESET
} TrapType_t;

#endif
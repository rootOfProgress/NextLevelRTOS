#ifndef EXCEPTION_H
#define EXCEPTION_H
#include <stdint.h>
#include "lang.h"

extern volatile uint32_t sp;
extern volatile uint32_t opcode;
extern volatile void* arg;

typedef enum {
    START_INIT_PROCESS = 0,
    ALLOC,
    DEVICE_REQUEST,
    RESET
} TRAP_TYPES;

typedef struct {
    void* arg;
    void* result;
} SVC_IO;

void trap();
// void INTERRUPT SVCall(uint32_t *);

#endif
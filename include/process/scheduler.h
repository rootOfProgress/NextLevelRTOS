#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>

typedef struct {
    short pid;
    uint32_t stackpointer_adress;
} TCB;

void run_init_process();
TCB* create_task(uint32_t, short);
#endif
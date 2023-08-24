#ifndef RUNTIME_TYPES_H
#define RUNTIME_TYPES_H

#include "gpio/gpio.h"

typedef enum KernelErrorCodes {
    SCHEDULER_INIT_FAILED,
    TASK_CREATION_FAILED,
    KERNEL_INIT_SUCCEDED
} KernelErrorCodes_t;

extern GpioObject_t general_error_led;








#endif
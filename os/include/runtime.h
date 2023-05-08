#ifndef RUNTIME_H
#define RUNTIME_H

typedef enum KernelErrorCodes {
    SCHEDULER_INIT_FAILED,
    TASK_CREATION_FAILED,
    KERNEL_INIT_SUCCEDED
} KernelErrorCodes_t;

void __attribute__((__noipa__)) __attribute__((optimize("O0"))) idle_runner(void);

#endif
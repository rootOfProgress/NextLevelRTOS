#ifndef PANIC_H
#define PANIC_H

typedef enum panic {
    OUT_OF_MEMORY = 0,
    SCHEDULER_NOT_INITIALIZED,
    MEMORY_DEALLOC_FAILED,
    ACCESS_ON_NULL
} PanicTypes_t;

void invoke_panic(PanicTypes_t panic_type);

#endif
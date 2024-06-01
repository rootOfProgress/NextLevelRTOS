#ifndef PANIC_H
#define PANIC_H

typedef enum panic
{
  NO_PANIC = 0,
  OUT_OF_MEMORY,
  SCHEDULER_NOT_INITIALIZED,
  MEMORY_DEALLOC_FAILED,
  ACCESS_ON_NULL
} PanicTypes_t;

extern PanicTypes_t panic_state;

void invoke_panic(PanicTypes_t panic_type);

#endif
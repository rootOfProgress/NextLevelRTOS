#ifndef RUNTIME_TYPES_H
#define RUNTIME_TYPES_H

typedef enum KernelErrorCodes
{
  SCHEDULER_INIT_FAILED,
  TASK_CREATION_FAILED,
  KERNEL_INIT_SUCCEDED
} KernelErrorCodes_t;

typedef enum OsErrorCodes
{
  OS_MemoryAllocationFailed = 1,
  OS_TaskInsertionFailed = 1 << 1,
  OS_RTCInitializationFailed = 1 << 2,
  OS_TaskQueueInitializationFailed = 1 << 3,
  OS_TaskCreationFailed = 1 << 4,
  OS_SchedulerInitializationFailed = 1 << 5,
} OsErrorCodes_t;



#endif
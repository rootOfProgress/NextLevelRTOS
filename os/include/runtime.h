#ifndef RUNTIME_H
#define RUNTIME_H

#define GIT_HASH 0xef40060c;
#define OS_VERSION 23032024;
#define GCC_VERSION 1320;

#include "runtime_types.h"

/**
 *
 * @param: None
 * @return: None
 *
 * This function is the kernels idle task and executed only under the following
 * circumstances:
 *
 * * there is no active runnable task in the queue. depending on enabled systick it is woken up
 * periodically, else it remains in sleep mode until an interrupt occurs.
 * * an external, schedules task was received by uart.
 *
 * On execution, finished or dead tasks will be cleaned up and, as described, an external
 * incoming task will be created.
 *
 */
void __attribute__((__noipa__)) __attribute__((optimize("O0"))) idle_runner(void);

/**
 * @brief Schedule a kernel subtask.
 *
 * This function schedules a kernel subtask identified by its task number.
 *
 * @param task_number The task number of the kernel subtask to be scheduled.
 */
void schedule_kernel_subtask(unsigned int task_number);

typedef enum IoChannel
{
  OsInternalIo,
  ModExternalIo
} IoChannel_t;

enum KernelTasks
{
  statisticManager = 0,
  transferManager = 1,
};

enum { maxNumOfWaitingKernelSubtasks = 3 };

extern void (*io_handler) (unsigned int uart_rx_buffer);
extern IoChannel_t type_of_io_handler;
extern unsigned int rx_state;

#endif
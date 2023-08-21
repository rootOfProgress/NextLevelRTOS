#ifndef RUNTIME_H
#define RUNTIME_H

#define GIT_HASH GIT_HASH_PARSED;
#define OS_VERSION 12062023;

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

typedef enum IoChannel {
    OsInternalIo,
    ModExternalIo
} IoChannel_t;

extern void (*io_handler) (unsigned int uart_rx_buffer);
extern IoChannel_t type_of_io_handler; 
extern unsigned int rx_state;

#endif
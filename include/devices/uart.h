#ifndef UART_H
#define UART_H

#include "lang.h"
#include "memory.h"
#include "process/task.h"

unsigned int read_data_register(void);
// extern void __attribute__((interrupt)) __attribute__ ((always_inline)) uart_isr_handler(void);
void init_uart(void);
void init_isr(void);

#define magic 0x123456

typedef enum {
    RX_READY = 0x0,
    PREPARE_TASK_TRANSFER = 0x1,
    TRANSFER_TASK_BYTES = 0x2,
} UartStates_t;

#endif
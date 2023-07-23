#ifndef UART_COMMON_H
#define UART_COMMON_H

#include "lang.h"
#include "memory.h"
#include "process/task.h"
#include "gpio.h"

#define MAGIC 0x123456
#define BUFFERSIZE 4
#define TX_LENGTH 64
#define MAX_WAITING_TRANSFERS 5

typedef struct transfer {
    void* start_adress;
    unsigned int length;
} TransferInfo_t;

typedef enum {
    RX_READY = 0x0,
    PREPARE_TASK_TRANSFER = 0x1,
    TRANSFER_TASK_BYTES = 0x2,
    REQUEST_STATISTIC = 0x3,
    ALTER_SPEED = 0x4,
    REQUEST_RPM = 0x5,
    REBOOT = 0x6,
    REQUEST_TEST_RESULT = 0x7,
    REQUEST_POSITION = 0x8
    // UNKNOWN = 0x3,
} UartStates_t;

void init_isr(void);
void setup_transfer(char*, unsigned int);
void init_transfer_handler(void);
void transfer_handler(void);
void external_io_runner(void);
#endif
#ifndef UART_H
#define UART_H

#include "lang.h"
#include "memory.h"
#include "process/task.h"
#include "gpio.h"

#define MAGIC 0x123456
#define BUFFERSIZE 4
#define TX_LENGTH 64
#define MAX_WAITING_TRANSFERS 10

typedef enum transfer_types {
    GENERIC = 0,
    MEM_ADDRESS,
    MEM_STAT,
    STATISTIC,
    RPM,
    PLANEPOSITION,
} TransferType_t;

typedef struct transfer {
    void* start_adress;
    unsigned int length;
    TransferType_t type;
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

extern void init_uart(GpioObject_t*);
extern void print(char*, unsigned int);
extern void print_char(char);

void init_isr(void);
void setup_transfer(char*, unsigned int, TransferType_t);
void init_transfer_handler(void);
void transfer_handler(void);
unsigned int read_data_register(void);

#endif
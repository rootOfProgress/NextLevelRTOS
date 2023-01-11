#ifndef UART_H
#define UART_H

#include "lang.h"
#include "memory.h"
#include "process/task.h"
#include "gpio.h"

#define magic 0x123456
#define stat_unlock 0x6789ab

typedef enum transfer_types {
    GENERIC = 0,
    MEM_ADDRESS,
    MEM_STATISTIC,
    SCHED_STATISTIC
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
    REQUEST_STATISTIC,
    ALTER_SPEED,
    UNKNOWN = 0x3,
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
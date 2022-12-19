#ifndef UART_H
#define UART_H

#include "lang.h"
#include "memory.h"
#include "process/task.h"
#include "gpio.h"

unsigned int read_data_register(void);
// extern void __attribute__((interrupt)) __attribute__ ((always_inline)) uart_isr_handler(void);
void init_isr(void);
extern void init_uart(GpioObject_t*);
extern void print(char*,unsigned int);
void setup_transfer(char*, unsigned int);
extern void print_char(char c);

void init_transfer_handler(void);
void transfer_handler(void);


#define magic 0x123456

typedef struct transfer {
    void* start_adress;
    unsigned int length;
} TransferInfo_t;

typedef enum {
    RX_READY = 0x0,
    PREPARE_TASK_TRANSFER = 0x1,
    TRANSFER_TASK_BYTES = 0x2,
} UartStates_t;

#endif
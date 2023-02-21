#ifndef UART_H
#define UART_H
#include "gpio.h"

#define USART1_BASEADRESS 0x40013800
#define UE 0
#define RE 2
#define TE 3
#define RXNEIE 5

typedef struct UartRegisterMap {
    unsigned int* cr1;
    unsigned int* cr2;
    unsigned int* cr3;
    unsigned int* brr;
    unsigned int* gtpr;
    unsigned int* rtor;
    unsigned int* rqr;
    unsigned int* isr;
    unsigned int* icr;
    unsigned int* rdr;
    unsigned int* tdr;
} UartRegisterMap_t;

// static inline __attribute__((always_inline)) unsigned int read_data_register(void);
// inline __attribute__((always_inline)) unsigned int read_data_register(void);
// inline __attribute__((always_inline)) unsigned int read_data_register(void)
// {
//     return *((unsigned int*) 0x40013824);   
// }
// static inline __attribute__((always_inline)) unsigned int read_data_register(void)
// {
//     return *((unsigned int*) 0x40013824);    
// }

void init_uart(GpioObject_t*);
void print(char *, unsigned int);
unsigned int read_data_register(void);

#endif
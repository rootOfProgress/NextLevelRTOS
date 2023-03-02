#ifndef UART_H
#define UART_H
#include "gpio.h"

enum { Usart1Baseadress = 0x40011000 };

#define RCC_USART1EN 4
#define TC 6
#define TXE 7

typedef struct UartRegisterMap {
    unsigned int sr;
    unsigned int dr;
    unsigned int brr;
    unsigned int cr1;
    unsigned int cr2;
    unsigned int cr3;
    unsigned int gtpr;
} UartRegisterMap_t;

void init_uart(GpioObject_t*);
void print(char*, unsigned int);
unsigned int read_data_register(void);


#endif
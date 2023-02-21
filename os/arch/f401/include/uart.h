#ifndef UART_H
#define UART_H
#include "gpio.h"

const unsigned int USART1_BASEADRESS = 0x40011000;

#define USART1EN 4

extern unsigned int UE;
extern unsigned int RE;
extern unsigned int TE;
extern unsigned int RXNEIE;

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
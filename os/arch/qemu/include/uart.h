#ifndef UART_H
#define UART_H
#include "gpio/gpio.h"
#include "lang.h"

enum { Usart1Baseadress = 0x40011000 };

typedef struct UartRegisterMap {
    unsigned int sr;
    unsigned int dr;
    unsigned int brr;
    unsigned int cr1;
    unsigned int cr2;
    unsigned int cr3;
    unsigned int gtpr;
} UartRegisterMap_t;

#define USART1_SR 0x40011000
#define USART1_sr 0x40011000
#define USART1_DR 0x40011004
#define USART1_CR 0x4001100C
#define UE 13
#define RXNEIE 5
#define RE 2
#define TE 3
#define BUFFERSIZE 4


void init_uart(GpioObject_t*);
void print_char(char c);


static inline __attribute__((always_inline)) char read_data_register(void)
{
    return (char) *((char*) USART1_DR);
}

static inline __attribute__((always_inline)) void print(char* src, unsigned int length)
{
    for (unsigned int i = 0; i < length; i++)
    {
        WRITE_REGISTER(USART1_DR,*src);
        while (!((READ_REGISTER(USART1_SR) & 0x80) != 0));
        src++;
    }
}
#endif
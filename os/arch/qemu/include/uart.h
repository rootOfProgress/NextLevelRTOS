#ifndef UART_H
#define UART_H
#include "gpio.h"
#include "lang.h"

// #include <stdint.h>

// #define READ_REGISTER(addr)     (*(volatile uint32_t *) (addr))
// #define WRITE_REGISTER(addr, val) ((*(volatile uint32_t *) (addr)) = (uint32_t) (val))
// #define WRITE_REGISTER64(addr, val) ((*(volatile uint64_t *) (addr)) = (uint64_t) (val))

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
// unsigned int read_data_register(void);

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
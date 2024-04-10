#ifndef UART_H
#define UART_H
#include "gpio/gpio.h"

enum { Usart1Baseadress = 0x40011000 };

#define RCC_USART1EN 4
#define TC 6
#define TXE 7
#define USART1_sr 0x40011000
#define USART1_cr1 0x4001100C
#define USART1_cr3 0x40011014

enum UartStatusBits
{
  ORE = 3,  //<! Overrun Error
  RXNE = 5, //<! Receive Register Not Empty
};

typedef struct UartRegisterMap
{
  unsigned int sr;
  unsigned int dr;
  unsigned int brr;
  unsigned int cr1;
  unsigned int cr2;
  unsigned int cr3;
  unsigned int gtpr;
} UartRegisterMap_t;

void init_uart(GpioObject_t*);
extern unsigned int* internal_rx_state;

static inline __attribute__((always_inline)) unsigned int read_data_register(void)
{
  return *((unsigned int*) (Usart1Baseadress | 0x04));
}

// @todo: horrible, plzzzz replace with dma send
static inline __attribute__((always_inline)) void print(char* src, unsigned int length)
{
  for (unsigned int i = 0; i < length; i++)
  {
    while (!((READ_REGISTER(&((UartRegisterMap_t * ) Usart1Baseadress)->sr) & (1 << TXE)) != 0));
    WRITE_REGISTER(&((UartRegisterMap_t*) Usart1Baseadress)->dr, *src);
    src++;
  }
  while (!(( READ_REGISTER(&((UartRegisterMap_t * ) Usart1Baseadress)->sr) & (1 << TC)) != 0));
}

#endif
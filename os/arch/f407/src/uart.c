#include "uart.h"

unsigned int UE = 0;
unsigned int RE = 0;
unsigned int TE = 0;
unsigned int RXNEIE = 0;

void init_uart(void)
{
  // RccRegisterMap_t* rcc_regs = (RccRegisterMap_t*) RCC_BASE_ADRESS;

  // // @todo
  // *rcc_regs->apb2enr = (*rcc_regs->apb2enr) | 1 << USART1EN;

  // // bus is feed from HSI CLK with 16 MHZ.
  // // how to set brr register:
  // // page 981 table 136 entry 2:
  // // baud: 104.1875 -> 104 | 16 * 0.1875 : 0d1667
  // UartRegisterMap_t* uart_regs = (UartRegisterMap_t*) USART1_BASEADRESS;
  // // unsigned int baudrate_divisor = 8000000 / 9600;

  // *uart_regs->brr = 1667;
  // *uart_regs->cr1 =  *uart_regs->cr1 | (0b1100);
  // *uart_regs->cr1 =  *uart_regs->cr1 | (0b1 << 13);

  // *((unsigned int*) 0xE000E104) = *((unsigned int*) 0xE000E104) | 1 << 5;
}

unsigned int read_data_register(void)
{
  return 0;
}

void print(char* src)
{
  return;
}
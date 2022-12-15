#include "uart.h"

void init_uart(void){
    *((unsigned int*) USART1_CR) = *((unsigned int*) USART1_CR) | 0x1 << UE | 0x1 << RXNEIE | 0x1 << RE | 0x1 << TE;
}

unsigned int read_data_register(void)
{
    return *((char*) USART1_DR);
}
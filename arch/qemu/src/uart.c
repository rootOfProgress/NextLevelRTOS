#include "uart.h"
#include "lang.h"

void init_uart(void){
    *((unsigned int*) USART1_CR) = *((unsigned int*) USART1_CR) | 0x1 << UE | 0x1 << RXNEIE | 0x1 << RE | 0x1 << TE;
    *((unsigned int*) 0xE000E104) = *((unsigned int*) 0xE000E104) | 0x1 << 5;

}

unsigned int read_data_register(void)
{
    return *((char*) USART1_DR);
}

void print_char(char c)
{
    WRITE_REGISTER(USART1_DR,(c));
    while (!((READ_REGISTER(USART1_SR) & 0x80) != 0));
}

void print(char* src, unsigned int length)
{
    for (unsigned int i = 0; i < length; i++)
    {
        WRITE_REGISTER(USART1_DR,(*src));
        while (!((READ_REGISTER(USART1_SR) & 0x80) != 0));
        src++;
    }
    send_nil();
}

void send_nil()
{
    char *nil = '\0';
    WRITE_REGISTER(USART1_DR,(nil));
    while (!((READ_REGISTER(USART1_SR) & 0x80) != 0));
}
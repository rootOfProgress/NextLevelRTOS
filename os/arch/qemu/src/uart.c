#include "uart.h"
#include "lang.h"
#include "gpio/gpio.h"

// @todo: invent cleaner solution (e.g. os_api.h)
#define SV_GET_IO_BUFFER __asm volatile ("mov r6, 9\n" \
                                  "mov r9, r0\n"\
                                  "svc 0\n")

unsigned int* internal_rx_state;

USED void get_State(UNUSED unsigned int** internal_rx_state )
{
  SV_GET_IO_BUFFER;
}

void NO_OPT init_uart(__attribute__((unused)) GpioObject_t* o)
{
    get_State(&internal_rx_state);
    *((unsigned int*) USART1_CR) = *((unsigned int*) USART1_CR) | 0x1 << UE | 0x1 << RXNEIE | 0x1 << RE | 0x1 << TE;
    *((unsigned int*) 0xE000E104) = *((unsigned int*) 0xE000E104) | 0x1 << 5;
}

void print_char(char c)
{
    WRITE_REGISTER(USART1_DR,(c));
    while (!((READ_REGISTER(USART1_SR) & 0x80) != 0));
}

// unsigned int read_data_register(void)
// {
//     return *((char*) USART1_DR);
// }

// void print(char* src, unsigned int length)
// {
//     for (unsigned int i = 0; i < length; i++)
//     {
//         WRITE_REGISTER(USART1_DR,*src);
//         while (!((READ_REGISTER(USART1_SR) & 0x80) != 0));
//         src++;
//     }
// }
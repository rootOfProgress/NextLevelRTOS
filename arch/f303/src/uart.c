#include "uart.h"
#include "rcc.h"

unsigned int UE = 0;
unsigned int RE = 2;
unsigned int TE = 3;
unsigned int RXNEIE = 5;

void init_uart(void)
{
    RccRegisterMap_t* rcc_regs = (RccRegisterMap_t*) RCC_BASE_ADRESS;
    *rcc_regs->apb2enr = (*rcc_regs->apb2enr) | 1 << USART1EN;

    UartRegisterMap_t* uart_regs = (UartRegisterMap_t*) USART1_BASEADRESS;
    unsigned int baudrate_divisor = 8000000 / 9600;

    *uart_regs->brr = baudrate_divisor;
    *uart_regs->cr1 =  *uart_regs->cr1 | (1 << TE | 1 << RE | 1 << RXNEIE | 1 << UE);
    *uart_regs->cr1 =  *uart_regs->cr1 | UE;


    *((unsigned int*) 0xE000E104) = *((unsigned int*) 0xE000E104) | 1 << 5;
}

unsigned int read_data_register(void)
{
    return *((char*) (((UartRegisterMap_t*) USART1_BASEADRESS)->rdr));
}
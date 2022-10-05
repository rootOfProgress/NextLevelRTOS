#include "../include/uart.h"

void send_nil();
void init_uart()
{
    uint32_t current_reg_content = READ_REGISTER(USART1_CR);
    WRITE_REGISTER(USART1_CR, current_reg_content | 0x1 << UE | 0x1 << RXNEIE | 0x1 << RE | 0x1 << TE);
}

void print_char(char c)
{
    WRITE_REGISTER(USART1_DR,(c));
    while (!((READ_REGISTER(USART1_SR) & 0x80) != 0));
}

void print(char *str)
{
    while (*str)
    {
        WRITE_REGISTER(USART1_DR,(*str));
        while (!((READ_REGISTER(USART1_SR) & 0x80) != 0));
        str++;
    }
}

void send_nil()
{
    char *nil = '\0';
    WRITE_REGISTER(USART1_DR,(nil));
    while (!((READ_REGISTER(USART1_SR) & 0x80) != 0));
}

void send_number(uint32_t number)
{
    uint32_t converted[20];
    uint32_t n = number;
    int cnt = 0;

    if (n == 0)
        print_char(0 + 0x30);
    else
    {
        while (n > 0)
        {
            converted[cnt] = (n % 10) + 0x30;
            n /= 10;
            cnt++;
        }
        for (int i = cnt - 1; i >= 0; i--)
        {
            print_char(converted[i]);
        }
    }
    print_char('\n');
}

uint32_t pow_base_16(uint32_t exp)
{
    uint32_t base = 16;
    if (exp == 0)
    {
        return 1;
    }
    else if (exp == 1)
    {
        return 16;
    }
    else
    {
        for (uint32_t n = 0; n < exp - 1; n++)
        {
            base *= 16;
        }
    }
    return base;
}

uint32_t hex2int(char ch)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    if (ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;
    if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;
    return -1;
}


void __attribute__((interrupt)) uart_isr_handler(void)
{
}


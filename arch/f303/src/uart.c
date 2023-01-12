#include "uart.h"
#include "rcc.h"
#include "lang.h"
#include "gpio.h"


void init_uart(GpioObject_t *obj)
{
    // tx
    obj->pin = 9;
    obj->port = 'A';
    init_gpio(obj); //passt

    set_moder(obj, AlternateFunctionMode);//passt
    into_af(obj, 7);
    set_otyper(obj, PushPull);

    // rx
    obj->pin = 10;
    obj->port = 'A';
    set_moder(obj, InputMode);
    set_moder(obj, AlternateFunctionMode);
    into_af(obj, 7);


    RccRegisterMap_t* rcc_regs = (RccRegisterMap_t*) RCC_BASE_ADRESS;
    WRITE_REGISTER(&rcc_regs->apb2enr, READ_REGISTER(&rcc_regs->apb2enr) | 1 << USART1EN);

    UartRegisterMap_t* uart_regs = (UartRegisterMap_t*) USART1_BASEADRESS;
    unsigned int baudrate_divisor = 8000000 / 9600;

    WRITE_REGISTER(&uart_regs->brr,baudrate_divisor);
    WRITE_REGISTER(&uart_regs->cr1, READ_REGISTER(&uart_regs->cr1) | (1 << TE | 1 << RE | 1 << RXNEIE | 1 << UE));
    WRITE_REGISTER(&uart_regs->cr1, READ_REGISTER(&uart_regs->cr1) | UE);


    *((unsigned int*) 0xE000E104) = *((unsigned int*) 0xE000E104) | 1 << 5;
}

unsigned int read_data_register(void)
{
    return *((unsigned int*) 0x40013824);
}

void print(char* src, unsigned int length)
{
    for (unsigned int i = 0; i < length; i++)
    {
        WRITE_REGISTER(&((UartRegisterMap_t*) USART1_BASEADRESS)->tdr,*src);
        while (!(( READ_REGISTER(&((UartRegisterMap_t*) USART1_BASEADRESS)->isr) & 0x80) != 0));
        src++;
    }
}
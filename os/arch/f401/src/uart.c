#include "uart.h"
#include "rcc.h"
#include "lang.h"
#include "gpio.h"
#include "dma.h"

void init_uart(GpioObject_t* obj)
{
    dma_init();

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



    RccRegisterMap_t* rcc_regs = (RccRegisterMap_t*) RCC_BASE;

    WRITE_REGISTER(&rcc_regs->apb2enr, READ_REGISTER(rcc_regs->apb2enr) | (1 << RCC_USART1EN));

    // bus is feed from HSI CLK with 16 MHZ.
    // how to set brr register:
    // page 981 table 136 entry 2:
    // baud: 104.1875 -> 104 | 16 * 0.1875 : 0d1667
    UartRegisterMap_t* uart_regs = (UartRegisterMap_t*) Usart1Baseadress;
    // unsigned int baudrate_divisor = 16000000 / 9600;

    WRITE_REGISTER(&uart_regs->brr, 1667);
    WRITE_REGISTER(&uart_regs->cr1, READ_REGISTER(&uart_regs->cr1) | (0b1100) | (1 << 5));
    WRITE_REGISTER(&uart_regs->cr1, READ_REGISTER(&uart_regs->cr1) | (0b1 << 13));


    // enable uart interrupt
    *((unsigned int*) 0xE000E104) = *((unsigned int*) 0xE000E104) | 1 << 5;
}
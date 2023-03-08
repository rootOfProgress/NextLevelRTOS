#include "fifo.h"
#include "gpio.h"
#include "spi.h"
#include "adxl345_common.h"
#include "rcc.h"

#define SV_YIELD_TASK __asm volatile ("mov r6, 2\n" \
                                  "svc 0\n")

#define SV_PRINT __asm volatile ("mov r6, 1\n" \
                                  "svc 0\n")
    

#define READ_REGISTER(addr)     (*(volatile unsigned int *) (addr))
#define WRITE_REGISTER(addr, val) ((*(volatile unsigned int *) (addr)) = (unsigned int) (val))


void init_spi_gpio(GpioObject_t* obj)
{
    init_gpio(obj); //passt

    // clk
    obj->pin = 3;
    obj->port = 'B';
    set_moder(obj, AlternateFunctionMode);//passt
    into_af(obj, 5);
    set_otyper(obj, OpenDrain);
    set_pupdr(obj, PullUp);
    set_speed(obj, High);

    // miso
    obj->pin = 4;
    obj->port = 'B';
    // set_moder(obj, InputMode);
    set_otyper(obj, PushPull);
    set_pupdr(obj, PullUp);
    set_speed(obj, High);
    set_moder(obj, AlternateFunctionMode);
    into_af(obj, 5);

    // mosi
    obj->pin = 5;
    obj->port = 'B';
    set_otyper(obj, OpenDrain);
    set_pupdr(obj, PullUp);
    set_speed(obj, High);
    set_moder(obj, AlternateFunctionMode);
    into_af(obj, 5);

    // csl
    obj->pin = 15;
    obj->port = 'A';
    set_pupdr(obj, PullUp);
    set_moder(obj, AlternateFunctionMode);
    into_af(obj, 5);

    // set_otyper(obj, PushPull);
    // set_pupdr(obj, PullUp);
// 
    // set_moder(obj, GeneralPurposeOutputMode);
    // set_pin_on(obj);

}

void init_spi_peripherial()
{
    GpioObject_t t;
    t.port = 'B';
    init_spi_gpio(&t);
    
    RccRegisterMap_t* rcc_regs = (RccRegisterMap_t*) RccBaseAdress;
    // WRITE_REGISTER(&rcc_regs->apb2rstr, READ_REGISTER(&rcc_regs->apb2enr) | (1 << SPI1EN));
    // WRITE_REGISTER(&rcc_regs->apb2rstr, READ_REGISTER(&rcc_regs->apb2enr) & ~(1 << SPI1EN));

    WRITE_REGISTER(&rcc_regs->apb2enr, READ_REGISTER(&rcc_regs->apb2enr) | (1 << SPI1EN));

//    
    SpiRegisterMap_t* spi_regs = (SpiRegisterMap_t*) Spi1BaseAdress;
    WRITE_REGISTER(&spi_regs->cr1, READ_REGISTER(&spi_regs->cr1) & ~(1 << SPE));
    // clear register
    WRITE_REGISTER(&spi_regs->cr1, ~0xFFFFFFFF);

    WRITE_REGISTER(&spi_regs->cr1, READ_REGISTER(&spi_regs->cr1) | (0 << DFF) | (7 << 3) | (1 << MSTR) | (1 << SSM) | (1 << SSI));
    // WRITE_REGISTER(&spi_regs->cr1, READ_REGISTER(&spi_regs->cr1) | (1 << DFF) | (7 << 3) | (1 << MSTR));
    // WRITE_REGISTER(&spi_regs->cr1, READ_REGISTER(&spi_regs->cr1) | (1 << SPE));

    WRITE_REGISTER(&spi_regs->cr2, READ_REGISTER(&spi_regs->cr2) | (1 << SSOE));


}

void transfer(char data) 
{
    SpiRegisterMap_t* spi_regs = (SpiRegisterMap_t*) Spi1BaseAdress;
    // __asm("bkpt");
    // char payload[3] = {0x01, 0x02, 0x03};
    char payload[3] = {0x01, 0x01, 0x01};
    unsigned int bar = READ_REGISTER(&spi_regs->dr);

    WRITE_REGISTER(&spi_regs->cr1, READ_REGISTER(&spi_regs->cr1) | (1 << SPE));

    for (unsigned int t = 0; t < 3; t++)
    {
        // unsigned short tx_data = ((unsigned short) data) << 8; 

        WRITE_REGISTER(&spi_regs->dr, payload[t]);
        while (!((READ_REGISTER(&(spi_regs)->sr) & (1 << BSY)) == 0));
        unsigned int foo = READ_REGISTER(&spi_regs->dr);
    }
    
    WRITE_REGISTER(&spi_regs->cr1, READ_REGISTER(&spi_regs->cr1) & ~(1 << SPE));
    // __asm("bkpt");

}

void __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{   

    GpioObject_t t;
    t.pin = 6;
    t.port = 'B';
    // set_pin_on(&t);

    init_spi_peripherial();
    transfer(0x3);
    // READ_REGISTER(&spi_regs->dr);

    // {
    //     /* code */
    
    // set_pin_off(&t);
    


    // WRITE_REGISTER(&spi_regs->cr1, READ_REGISTER(&spi_regs->cr1) & ~(1 << SPE));
    // set_pin_on(&t);
    
    // set_pin_off(&t);
    //     while (!((READ_REGISTER(&(spi_regs)->sr) & (1 << TXE)) != 0));
    //     while (!((READ_REGISTER(&(spi_regs)->sr) & (1 << BSY)) != 1));
    //     READ_REGISTER(&spi_regs->dr);

    // }
    // // READ_REGISTER(&spi_regs->dr);

    // __asm("bkpt");
// 
    // set_pin_off(&t);
    // set_pin_on(&t);

    // while (1)
    // {
    //     SV_YIELD_TASK;
    // }
    

}

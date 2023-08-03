#include "os_api.h"
#include "adc.h"


void adc_isr()
{
    adc_acknowledge_interrupt();
    unsigned int value = adc_read();
    asm("bkpt");  
}

int __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{   
    GpioObject_t gpio;
    gpio.pin = 1;
    gpio.port = 'A';
    adc_init(&gpio);
    adc_turn_on();
    link_adc_isr(adc_isr);
    adc_start_conv_regular();
    // while (1)
    // {
    //     // adc_start_conv_injected();
    //     svcall(yieldTask);
    // }
    
    return 0;
}

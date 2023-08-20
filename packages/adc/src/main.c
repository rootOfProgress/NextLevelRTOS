#include "os_api.h"
#include "adc.h"

static unsigned int rounds;

void adc_isr()
{
    adc_acknowledge_interrupt();
    unsigned int value = adc_read_regular_channel();
    asm("bkpt");  
    // rounds++;
}

int __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{   
    rounds = 0;
    GpioObject_t gpio;
    gpio.pin = 1;
    gpio.port = 'A';
    adc_init(&gpio);
    adc_enable_interrupts();

    adc_turn_on();
    link_adc_isr(adc_isr);
    adc_setbit_cr2_generic(1);
    adc_start_conv_regular();
    
    // while (1)
    // {
        // @todo: just block task, isr should wake it.
        // or: sleep
        // svcall(yieldTask);
    // }
    // sleep(1000);
    // asm("bkpt");  
    
    return 0;
}

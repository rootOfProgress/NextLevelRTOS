#include "gpio.h"
#include "test.h"
#include "spi.h"
#include "nrf24l01.h"
#include "nrf_driver.h"
#include "rcc.h"
#include "uart.h"
#include "os_api.h"

#define SV_YIELD_TASK __asm volatile ("mov r6, 2\n" \
                                  "svc 0\n")


#define READ_REGISTER(addr)     (*(volatile unsigned int *) (addr))
#define WRITE_REGISTER(addr, val) ((*(volatile unsigned int *) (addr)) = (unsigned int) (val))


char receive_buffer[RX_BUFFER_SIZE];
char tx_buffer[TX_BUFFER_SIZE];
Nrf24l01Registers_t nrf24l01_regs;


void apply_nrf_config(Nrf24l01Registers_t *nrf_registers)
{

    /************ Disable AA ***************/
    nrf_registers->en_aa = 0;

    /************ 1Mbps data rate, 0dBm ***************/
    nrf_registers->rf_setup = 6;

    /************ 5 byte address width ***************/
    nrf_registers->setup_aw = 3;
    
    /************ Channel 2 ***************/
    nrf_registers->rf_ch = 2;

    // LSB is written first, will result in bfcecccecc
    // char tx[5] = {0xCC, 0xCE, 0xCC, 0xCE, 0xBF};
    char tx[5] = {0x9A, 0x78, 0x56, 0x34, 0x12};
    for (unsigned int i = 0; i < sizeof(tx)/sizeof(char); i++)
        nrf_registers->tx_addr[i] = tx[i];
    // for (int i = sizeof(tx)/sizeof(char) - 1; i >= 0 ; i--)

    char rx_p0[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
    for (unsigned int i = 0; i < sizeof(rx_p0)/sizeof(char); i++)
        nrf_registers->rx_addr_p0[i] = rx_p0[i];
    // for (int i = sizeof(rx_p0)/sizeof(char) - 1; i >= 0; i--)
}

int __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{   
    init_spi();
    GpioObject_t orange_led;
    GpioObject_t blue_led;

    orange_led.pin = 14;
    orange_led.port = 'C';

    blue_led.pin = 15;
    blue_led.port = 'C';

    init_gpio(&orange_led);
    init_gpio(&blue_led);

    set_moder(&orange_led, GeneralPurposeOutputMode);
    set_moder(&blue_led, GeneralPurposeOutputMode);


    set_pin_off(&blue_led);
    set_pin_off(&orange_led);

    
    Nrf24l01Registers_t nrf_registers;
    apply_nrf_config(&nrf_registers);

    memset_byte((void*) &receive_buffer, sizeof(receive_buffer), 0x0);
    memset_byte((void*) &current_nrf_config, sizeof(current_nrf_config), 0x0);
    configure_device(&nrf_registers, SLAVE);
    nrf_flush_rx();
    nrf_receive();

    while (1)
    {
        // unset_ce();
        get_nrf_config();

        if (current_nrf_config.rpd == (char) 1)
            set_pin_on(&orange_led);
        else
            set_pin_off(&orange_led);
        
        if ((current_nrf_config.status & (char) (1 << 6)) != 0)
        {
            set_pin_on(&blue_led);
            // unset_ce();
            char dummy[3] = {0xFF,0xFF,0xFF,};
            nrf_receive_payload(dummy, 3);
// 
            clear_rx_dr_flag();
            // nrf_receive();

        }
        else
        {
            asm("bkpt");
            set_pin_off(&blue_led);
        }



        SV_YIELD_TASK;
    }
    
    
    return 0;
}

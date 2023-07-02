#include "gpio.h"
#include "test.h"
#include "spi.h"
#include "nrf24l01.h"
#include "rcc.h"
#include "uart.h"
#include "os_api.h"

#define SV_YIELD_TASK __asm volatile ("mov r6, 2\n" \
                                  "svc 0\n")


#define READ_REGISTER(addr)     (*(volatile unsigned int *) (addr))
#define WRITE_REGISTER(addr, val) ((*(volatile unsigned int *) (addr)) = (unsigned int) (val))



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
    nrf_registers->rf_ch = 3;

    nrf_registers->en_rxaddr = (1 << 1) | (1);
    nrf_registers->rx_pw_p0 = 6;
    nrf_registers->rx_pw_p1 = 6;

    // LSB is written first, will result in bfcecccecc
    // char tx[5] = {0xCC, 0xCE, 0xCC, 0xCE, 0xBF};
    char tx[5] = {0x9A, 0x78, 0x56, 0x34, 0x12};
    for (unsigned int i = 0; i < sizeof(tx)/sizeof(char); i++)
        nrf_registers->tx_addr[i] = tx[i];
    // for (int i = sizeof(tx)/sizeof(char) - 1; i >= 0 ; i--)

    char rx_p0[5] = {0xc6, 0xc6, 0xc6, 0xc6, 0xc6};
    for (unsigned int i = 0; i < 5 /* sizeof(rx_p1)/sizeof(char) */; i++)
        nrf_registers->rx_addr_p0[i] = rx_p0[i];

    char rx_p1[5] = {0xc5, 0xc5, 0xc5, 0xc5, 0xc5};
    for (unsigned int i = 0; i < 5 /* sizeof(rx_p1)/sizeof(char) */; i++)
        nrf_registers->rx_addr_p1[i] = rx_p1[i];
    // for (int i = sizeof(rx_p0)/sizeof(char) - 1; i >= 0; i--)
}

typedef struct rxinfo {
    unsigned int pipe;
    char content[32];

} rxinfo_t;

rxinfo_t rx_data[32];
static unsigned int index = 0;

int __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{   
    // driver handels that
    // init_spi();
    index = 0;
    GpioObject_t orange_led;
    GpioObject_t blue_led;

    blue_led.pin = 14;
    blue_led.port = 'C';
    
    orange_led.pin = 15;
    orange_led.port = 'C';


    init_gpio(&orange_led);
    init_gpio(&blue_led);

    set_moder(&orange_led, GeneralPurposeOutputMode);
    set_moder(&blue_led, GeneralPurposeOutputMode);


    set_pin_off(&blue_led);
    set_pin_off(&orange_led);

    
    Nrf24l01Registers_t nrf_startup_config;
    Nrf24l01Registers_t nrf_current_config;


    memset_byte((void*) &nrf_current_config, sizeof(Nrf24l01Registers_t), 0x0);
    memset_byte((void*) &nrf_startup_config, sizeof(Nrf24l01Registers_t), 0x0);

    apply_nrf_config(&nrf_startup_config);
    configure_device(&nrf_startup_config, SLAVE);
    sleep(10);
    
    nrf_flush_rx();

    start_listening();
    char rx_answer[16];


    sleep(10);
    while (1)
    {
        sleep(1);
        while (!(get_nrf_fifo() & 1))
        {
            if (check_for_received_data(&nrf_startup_config, rx_answer))
            {
                asm("bkpt");
            }
        }

        // get_nrf_config(&nrf_cfg);
        // todo: check first if fifo is empty
        // clear_rx_dr_flag();
        // start_listening();
        sleep(1);


        SV_YIELD_TASK;
    }
    
    
    return 0;
}

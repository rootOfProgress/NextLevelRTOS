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
char rx_answer[16];
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

void process()
{

}

int __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{   
    init_spi();
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

    
    Nrf24l01Registers_t nrf_registers;
    Nrf24l01Registers_t nrf_cfg;

    memset_byte((void*) &receive_buffer, sizeof(RX_BUFFER_SIZE), 0x0);
    memset_byte((void*) &nrf_cfg, sizeof(Nrf24l01Registers_t), 0x0);
    memset_byte((void*) &nrf_registers, sizeof(Nrf24l01Registers_t), 0x0);

    apply_nrf_config(&nrf_registers);
    configure_device(&nrf_registers, SLAVE);
    sleep(10);
    

    // set_bit_nrf_register(CONFIG, 6);
    // set_bit_nrf_register(CONFIG, 5);
    // set_bit_nrf_register(CONFIG, 4);
    // nrf_receive();

    // nrf_flush_rx();

    start_listening();

    sleep(10);
    while (1)
    {
        // unset_ce();
        get_nrf_config(&nrf_cfg);

        if (nrf_cfg.rpd == (char) 1)
        {
            set_pin_on(&orange_led);
        }
        else 
        {
            set_pin_off(&orange_led);
        }

        int pipe = 7;
        pipe = (nrf_cfg.status >> 1) & 0x7;
        
        // if ((nrf_cfg.status & (char) (1 << 6)) != 0 ||)
        // {
            if(pipe >= 0 && pipe <= 5)
            {
                set_pin_on(&blue_led);
                stop_listening();
                sleep(1);
                // unset_ce();
                rx_data[index % 32].pipe = pipe;
                index++;
                char dummy[6] = {0xFF,0xFF,0xFF,};
                nrf_receive_payload(dummy, 6);
                start_listening();
                sleep(1);
                // nrf_receive();

            }
            else
            {

                set_pin_off(&blue_led);
            }
            clear_rx_dr_flag();
        // }
        // else
        // {

        //     set_pin_off(&blue_led);
        // }
        // if (nrf_cfg.rpd == (char) 1)
        // {
        //     unset_ce();
        //     sleep(10);
        //     // nrf_receive();
        //     set_pin_off(&orange_led);
        // }

        SV_YIELD_TASK;
    }
    
    
    return 0;
}

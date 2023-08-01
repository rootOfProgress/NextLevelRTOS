#include "nrf24l01.h"
#include "os_api.h"
#include "rcc.h"
#include "gpio/gpio.h"
#include "uart.h"
#include "tim2_5.h"
#include "spi.h"
#include "uart.h"
#include "exti.h"
#include "syscfg.h"

typedef enum droneCmds {
    adjustRpm = 0x30
} droneCmds_t;

#define READ_REGISTER(addr)     (*(volatile unsigned int *) (addr))
#define WRITE_REGISTER(addr, val) ((*(volatile unsigned int *) (addr)) = (unsigned int) (val))



Nrf24l01Registers_t nrf24l01_regs;
Nrf24l01Registers_t nrf_startup_config;
// char rx_answer[16];

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
    nrf_registers->rx_pw_p0 = 4;
    nrf_registers->rx_pw_p1 = 4;

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

// @warning: this is executed in handler mode with MSP
// variant 1: call handler function directly and pass data 
// variant 2: store data in global variable and wakeup io_external_runner (is just a thread)
// what is better?
void rx_receive_isr()
{
    char rx_answer[16];

    exti_acknowledge_interrupt(0);
    while (!(get_nrf_fifo() & 1))
    {
        if (check_for_received_data(&nrf_startup_config, rx_answer))
        {   
            // @workaround
            for (int i = 0; i < 4; i++)
            {
                rx_answer[i] = rx_answer[i+1];
            }
            
            unsigned int *rx_data = (unsigned int*) rx_answer;

            // @workaround
            asm("bkpt");
            unsigned int num = (unsigned int) *(rx_data); 
            unsigned int swapped = ((num>>24)&0xff) | // move byte 3 to byte 0
                    ((num<<8)&0xff0000) | // move byte 1 to byte 2
                    ((num>>8)&0xff00) | // move byte 2 to byte 1
                    ((num<<24)&0xff000000); // byte 0 to byte 3
            extIoHandler(swapped);

            // @ todo: concept questionable, because every module should have its own isr handler?
/*             ReceiveHeader_t *header = (ReceiveHeader_t*) rx_answer+1;
            switch (header->package_type)
            {
            case OsRequest:
                // copy request into uart_rx_buffer (@todo: Make that more general)
                break;
            case GeneralRequest:
            default:
                break;
            }
 */
        }
        else 
        {
            break;
        }
    }
    clear_rx_dr_flag();

}

void enable_nvic_interrupt(unsigned nvic_number)
{
    asm("bkpt");
    // WRITE_REGISTER(0xE000EF00, 0x6);
}


void extIoHandler(unsigned int uart_rx_buffer) 
{
    droneCmds_t state = (droneCmds_t) uart_rx_buffer & 0xFF;
    switch (state)
    {
    case adjustRpm:
        // format: 
        // [31 .. 30] | [29 .. 8] | [7 .. 0]
        //   CCR_NO       VALUE       STATE   

        unsigned int ccr_no = uart_rx_buffer >> 24;       
        unsigned int ccr_value = (uart_rx_buffer >> 8) & 0xFFFF;       
        asm("bkpt");
        set_ccr(2, ccr_value, ccr_no);
        
        break;
    
    default:
        break;
    }

   
}

int __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{   
    svcall_param(setExtIoCallback, (unsigned int) &extIoHandler);
    char rx_answer[16];
    index = 0;
    
    // Nrf24l01Registers_t nrf_startup_config;
    Nrf24l01Registers_t nrf_current_config;

    memset_byte((void*) &nrf_current_config, sizeof(Nrf24l01Registers_t), 0x0);
    memset_byte((void*) &nrf_startup_config, sizeof(Nrf24l01Registers_t), 0x0);

    apply_nrf_config(&nrf_startup_config);
    configure_device(&nrf_startup_config, SLAVE);
    sleep(10);

    nrf_flush_rx();
    clear_rx_dr_flag();

    start_listening();

    GpioObject_t pinb;
    pinb.pin = 0;
    pinb.port = 'B';

    link_exti_src(rx_receive_isr, &pinb);
    syscfg_enable_clock();
    syscfg_exti_config_0_3(&pinb);
    exti_activate_ir_line(&pinb);
    exti_detect_falling_edge(&pinb);
    timer_pwm_init(2, 8000);
    timer_start(2);
    while (1)
    {
        svcall(yieldTask);
    }
    

    return 0;
}

#define SV_YIELD_TASK __asm volatile ("mov r6, 2\n" \
                                  "svc 0\n")

#include "os_lib.h"
#include "nrf24l01.h"
#include "am2302.h"

Nrf24l01Registers_t nrf24l01_regs;

extern void memset_byte(void*, unsigned int, char);

typedef struct NodeFrame {
    char id;
    Am2302Readings_t readings;
} NodeFrame_t;

NodeFrame_t node_frame;

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

void __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) weathernode(void)
{   
    Nrf24l01Registers_t nrf_registers;
    apply_nrf_config(&nrf_registers);
    configure_device(&nrf_registers, MASTER);
    
    memset_byte((void*) &node_frame, sizeof(NodeFrame_t), 0);

    node_frame.id = 0xA0;

    am2302_init_peripherials(13, 'C');

    while (1)
    {
        am2302_do_measurement(&node_frame.readings);
        if (node_frame.readings.is_valid)
            nrf_transmit((char*) &node_frame, sizeof(NodeFrame_t));
        task_sleep(5000);
    } 
}
#include "os_api.h"
#include "adc.h"
#include "crc.h"
#include "remote.h"
#include "nrf24l01.h"

static unsigned int current_reading;
static unsigned int lower_bound;
static unsigned int upper_bound;

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


void set_lower_bound(void)
{
    // @todo: activate as soon as button, gpio isr is available
    // lower_bound = current_reading;
    lower_bound = 1000;
}

void set_upper_bound(void)
{
    // @todo: activate as soon as button, gpio isr is available
    // upper_bound = current_reading;
    upper_bound = 2500;
}

void adc_isr()
{
    adc_acknowledge_interrupt();
    current_reading = adc_read_regular_channel();
}

void init_adc()
{
    GpioObject_t gpio;
    gpio.pin = 1;
    gpio.port = 'A';
    adc_init(&gpio);

    // @todo: not sure if needed
    adc_enable_interrupts();
    link_adc_isr(adc_isr);

    adc_turn_on();

    // CONT: Continuous conversion
    adc_setbit_cr2_generic(1);
    
    adc_start_conv_regular();    
}

void main_routine(void)
{
    TxPayload_t tx_payload;
    memset_byte((void*) &tx_payload, sizeof(TxPayload_t), 0);

    while (1)
    {
        adc_disable_interrupts();

        if (current_reading > upper_bound)
        {
            current_reading = upper_bound; 
        }
        else if (current_reading < lower_bound)
        {
            current_reading = lower_bound;
        }
        
        crc_reset();
        crc_feed(AdjustSpeed);
        crc_feed(current_reading);
        tx_payload.cmd_type = (unsigned int) AdjustSpeed;
        tx_payload.cmd_argument = current_reading;
        tx_payload.checksum = (unsigned int) crc_read();
        nrf_transmit(&tx_payload, 13);

        adc_enable_interrupts();

        sleep(200);
    }
}

void config_remote_controller(void)
{
    Nrf24l01Registers_t nrf_registers;
    
    /* CRC Config */
    crc_activate();
    crc_reset();
    
    /* ADC Config */
    init_adc();

    /* NRF2401 Config */
    apply_nrf_config(&nrf_registers);
    configure_device(&nrf_registers, MASTER);
}



int __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{   
    config_remote_controller();
    set_lower_bound();
    set_upper_bound();
    main_routine();

  

    return 0;
}

#include "os_api.h"
#include "adc.h"
#include "crc.h"
#include "remote.h"
#include "nrf24l01.h"
#include "exti.h"
#include "syscfg.h"

int current_reading;
int lower_bound;
int upper_bound;
unsigned int mutex;
TxPayload_t tx_payload;
GpioObject_t mode_switch;
OperatingState_t operating_state;

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

void isr_switch_state(void)
{
    asm("bkpt");
    exti_acknowledge_interrupt(&mode_switch);
    // #define isRisingEdge 1
    // #define isFallingEdge 1
    // if (isRisingEdge)
    // {
    //     operating_state = Running;
    // }
    // if (isFallingEdge)
    // {
    //     operating_state = Maintenance;
    // }

    // @todo acknowledge exti interrupt

}


void fake_gpio_isr1(void)
{
    set_upper_bound();
}

void fake_gpio_isr2(void)
{
    set_lower_bound();
}


void adc_isr()
{
    adc_acknowledge_interrupt();

    lock_mutex(&mutex);
    current_reading = adc_read_regular_channel();
    release_mutex(&mutex);
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
    while (1)
    {
        adc_disable_interrupts();

        lock_mutex(&mutex);
        if (current_reading > upper_bound)
        {
            current_reading = upper_bound; 
        }
        else if (current_reading < lower_bound)
        {
            current_reading = lower_bound;
        }
        release_mutex(&mutex);

        crc_reset();
        crc_feed(AdjustSpeed);
        crc_feed(current_reading);
        tx_payload.cmd_type = (unsigned int) AdjustSpeed;
        tx_payload.cmd_argument = current_reading;
        tx_payload.checksum = (unsigned int) crc_read();
        nrf_transmit(&tx_payload, 13);
        adc_enable_interrupts();

        sleep(50);
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

    /* Set up ISR Routines */

    // @todo: ports randomly chosen
    mode_switch.pin = 1;
    mode_switch.port = 'B';

    // @todo: ports randomly chosen
    // GpioObject_t mode_set;
    // mode_switch.pin = 2;
    // mode_switch.port = 'B';

    init_gpio(&mode_switch);
    link_exti_src(isr_switch_state, &mode_switch);
    syscfg_enable_clock();
    syscfg_exti_config_0_3(&mode_switch);
    exti_activate_ir_line(&mode_switch);
    exti_detect_falling_edge(&mode_switch);

    // init_gpio(&mode_set);

    // // @todo: ports randomly chosen
    // GpioObject_t mode_led;
    // mode_switch.pin = 2;
    // mode_switch.port = 'D';
}



int __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{   
    config_remote_controller();
    
    // @todo: currently faked until isr is set up
    set_lower_bound();
    set_upper_bound();
    memset_byte((void*) &tx_payload, sizeof(TxPayload_t), 0);

    operating_state = Maintenance;
    mutex = 0;
    current_reading = 0;

    while (1)
    {
        switch (operating_state)
        {
        case Running:
            main_routine();
            break;
        // served by isr
        case Maintenance:
            svcall(yieldTask);
            break;
        default:
            break;
        }
    }  

    return 0;
}

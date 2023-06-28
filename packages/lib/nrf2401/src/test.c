#include "test.h"
#include "os_api.h"
#include "uart.h"
#include "nrf_driver.h"

typedef struct MeasurementResults {
    char Subtest000_000_check_default_config;
    char Subtest000_001_check_default_status;
    char Subtest000_002_check_power_on;
    char Subtest000_002_check_tx_addr_rw;
    char reserved[28]; 
} MeasurementResults_t;

char set_and_test_txaddr(void)
{
    char payload_tx_addr[5] = {0xCC, 0xCE, 0xCC, 0xCE, 0xCC};
    transfer(TX_ADDR, payload_tx_addr, sizeof(payload_tx_addr) / sizeof(payload_tx_addr[0]), WriteRegister, (void*) 0);

    // validate
    transfer(TX_ADDR, (char[5]) {0,0,0,0,0}, 5, ReadRegister, (void*) 0);
    for (int i = 0; i < 5; i++)
    {
        if (receive_buffer[i+1] != payload_tx_addr[i])
            return 0; 
    }
    
    return 1;
}

char get_and_test_nrf_register(Nrf24l01RegisterNames_t reg_type, char expected)
{
    char foo[1] = {0}; 
    transfer(reg_type, foo, 1, ReadRegister, (void*) 0);
    

    if (receive_buffer[0] == expected)
        return 1;
    return 0;
}

void self_check(void)
{
    MeasurementResults_t measurements;
    memset_byte((void*) &measurements, sizeof(MeasurementResults_t), 0);
    nrf_power_off();
    measurements.Subtest000_000_check_default_config = get_nrf_register(CONFIG) == 8 ? 1 : 0;
    measurements.Subtest000_001_check_default_status = get_nrf_register(STATUS) == 0xE ? 1 : 0;

    // // test power on
    set_bit_nrf_register(CONFIG, 1);
    measurements.Subtest000_002_check_power_on = (get_nrf_register(CONFIG) & (1 << 1)) ? 1 : 0;
    measurements.Subtest000_002_check_tx_addr_rw = set_and_test_txaddr();
    print((void*) &measurements, 32 * sizeof(char));
}
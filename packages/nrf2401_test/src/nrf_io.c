#include "nrf24l01.h"
#include "spi.h"
#include "os_api.h"
#include "nrf_driver.h"

void transfer(char target_register, char *data, unsigned int length, TransferType_t t) 
{
    memset_byte((void*) tx_buffer, TX_BUFFER_SIZE, 0);
    switch (t)
    {
    case read_register:
        tx_buffer[0] = target_register;
        break;
    case write_register:
        tx_buffer[0] = ((char) (1 << 5) | target_register);
        break;
    // don't care
    case r_rx_payload:
    case w_tx_payload:
        tx_buffer[0] = W_TX_PAYLOAD;
        break;
    default:
        return;
    }
    
    for (unsigned int i = 0; i < length && i < TX_BUFFER_SIZE; i++)
        tx_buffer[i+1] = data[i];

    spi_write(tx_buffer, length + 1, receive_buffer);
}

char get_nrf_register(Nrf24l01RegisterNames_t reg_type)
{
    char empty = 0;
    transfer(reg_type, &empty, sizeof(char), read_register);
    
    return receive_buffer[1];
}

void get_nrf_register_long(Nrf24l01RegisterNames_t reg_type, char* register_long)
{
    char empty[5] = {0,0,0,0,0};
    
    transfer(reg_type, empty, sizeof(empty)/sizeof(char), read_register);

    for (int i = 0; i < 5; i++)
    {
        register_long[i] = receive_buffer[i+1];
    }
}

void set_bit_nrf_register(Nrf24l01RegisterNames_t reg_type, char bit_position)
{
    char current_value = get_nrf_register(reg_type);
    char new_value = { current_value | 1 << bit_position }; 
    transfer(reg_type, &new_value, 1, write_register);   
}

void clear_bit_nrf_register(Nrf24l01RegisterNames_t reg_type, char bit_position)
{
    char current_value = get_nrf_register(reg_type);
    char new_value = { current_value & ~(1 << bit_position) }; 
    // asm("bkpt");
    transfer(reg_type, &new_value, 1, write_register);   
}

void replace_nrf_register(Nrf24l01RegisterNames_t reg_type, char new_value)
{
    char new_value_local = new_value;
    transfer(reg_type, &new_value_local, 1, write_register);
}

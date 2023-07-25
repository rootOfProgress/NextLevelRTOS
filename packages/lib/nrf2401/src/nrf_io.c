#include "nrf24l01.h"
#include "spi.h"
#include "nrf24l01_privates.h"

char rx_buffer[RX_BUFFER_SIZE];

void transfer_write(char target_register, unsigned int length, TransferType_t t, char* payload)
{
    char tx_buffer[PackageBufferSize];

    configure_package_type(target_register, t, tx_buffer);

    for (unsigned int i = 0; i < length && i < TX_BUFFER_SIZE; i++)
        tx_buffer[i+1] = payload[i];

    spi_write(tx_buffer, length + 1, rx_buffer);
}

void transfer_read(char target_register, unsigned int length, TransferType_t t)
{
    char tx_buffer[PackageBufferSize];

    configure_package_type(target_register, t, tx_buffer);

    for (unsigned int i = 0; i < length && i < TX_BUFFER_SIZE; i++)
        tx_buffer[i+1] = 0;

    spi_write(tx_buffer, length + 1, rx_buffer);
}

void transfer_read_wbuffer(char target_register, unsigned int length, TransferType_t t, char* custom_buffer)
{
    char tx_buffer[PackageBufferSize];

    configure_package_type(target_register, t, tx_buffer);

    for (unsigned int i = 0; i < length && i < TX_BUFFER_SIZE; i++)
        tx_buffer[i+1] = 0;

    spi_write(tx_buffer, length + 1, custom_buffer);
}

char get_nrf_register(Nrf24l01RegisterNames_t reg_type)
{
    transfer_read(reg_type, sizeof(char), ReadRegister);
    
    return rx_buffer[1];
}

void get_nrf_register_long(Nrf24l01RegisterNames_t reg_type, char* register_long)
{
    transfer_read(reg_type, 5 * sizeof(char), ReadRegister);

    for (unsigned int i = 0; i <  5 * sizeof(char); i++)
    {
        register_long[i] = rx_buffer[i+1];
    }
}

void set_nrf_register_long(Nrf24l01RegisterNames_t reg_type, char* payload)
{
    transfer_write(reg_type, 5, WriteRegister, payload);
}

void set_bit_nrf_register(Nrf24l01RegisterNames_t reg_type, char bit_position)
{
    char current_value = get_nrf_register(reg_type);
    char new_value = { current_value | 1 << bit_position }; 
    transfer_write(reg_type, 1, WriteRegister, &new_value);   
}

void clear_bit_nrf_register(Nrf24l01RegisterNames_t reg_type, char bit_position)
{
    char current_value = get_nrf_register(reg_type);
    char new_value = { current_value & ~(1 << bit_position) }; 
    transfer_write(reg_type, 1, WriteRegister, &new_value);   
}

void replace_nrf_register(Nrf24l01RegisterNames_t reg_type, char new_value)
{
    transfer_write(reg_type, 1, WriteRegister, &new_value);   
}

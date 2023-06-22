#include "nrf24l01.h"
#include "spi.h"
#include "os_api.h"
#include "nrf_driver.h"

void transfer(char target_register, char *data, unsigned int length, TransferType_t t) 
{
    memset_byte((void*) tx_buffer, TX_BUFFER_SIZE, 0);
    switch (t)
    {
    case ReadRegister:
        tx_buffer[0] = target_register & ~(0b111 << 5);
        break;
    case WriteRegister:
        tx_buffer[0] = ((char) (1 << 5) | target_register) & ~(0b11 << 6);
        break;
    case RRxPayload:
        tx_buffer[0] = R_RX_PAYLOAD;
        return;
    case WTxPayload:
        tx_buffer[0] = W_TX_PAYLOAD;
        break;
    case FlushTX:
        tx_buffer[0] = FLUSH_TX;
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
    transfer(reg_type, &empty, sizeof(char), ReadRegister);
    
    return receive_buffer[1];
}

void get_nrf_register_long(Nrf24l01RegisterNames_t reg_type, char* register_long)
{
    char empty[5] = {0,0,0,0,0};
    
    transfer(reg_type, empty, sizeof(empty)/sizeof(char), ReadRegister);

    for (int i = 0; i < 5; i++)
    {
        register_long[i] = receive_buffer[i+1];
    }
}

void set_nrf_register_long(Nrf24l01RegisterNames_t reg_type, char* register_long)
{
    transfer(reg_type, register_long, 5 /* sizeof(register_long)/sizeof(char) */, WriteRegister);
}

void set_bit_nrf_register(Nrf24l01RegisterNames_t reg_type, char bit_position)
{
    char current_value = get_nrf_register(reg_type);
    char new_value = { current_value | 1 << bit_position }; 
    transfer(reg_type, &new_value, 1, WriteRegister);   
}

void clear_bit_nrf_register(Nrf24l01RegisterNames_t reg_type, char bit_position)
{
    char current_value = get_nrf_register(reg_type);
    char new_value = { current_value & ~(1 << bit_position) }; 
    // asm("bkpt");
    transfer(reg_type, &new_value, 1, WriteRegister);   
}

void replace_nrf_register(Nrf24l01RegisterNames_t reg_type, char new_value)
{
    transfer(reg_type, &new_value, 1, WriteRegister);
}

#include "nrf24l01.h"

char get_nrf_register(Nrf24l01RegisterNames_t reg_type)
{
    char empty = 0;
    transfer(reg_type, &empty, sizeof(char), read_register);
    
    return receive_buffer[1];
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

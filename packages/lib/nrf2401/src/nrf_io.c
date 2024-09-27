#include "nrf24l01.h"
#include "spi.h"
#include "nrf24l01_privates.h"
#define NULL ( (void *) 0)

char rx_buffer[RX_BUFFER_SIZE];

char* transfer(char target_register,
               unsigned int length,
               TransferType_t transferType,
               char* buffer)
{
  char tx_buffer[PackageBufferSize];

  configure_package_type(target_register, transferType, tx_buffer);

  for (unsigned int i = 0; i < length && i < PackageBufferSize; i++)
  {
    tx_buffer[i + 1] = (transferType != RRxPayload) ? buffer[i] : 0;
  }

  switch (transferType)
  {
  case WTxPayload:
  case WriteRegister:
  case FlushTX:
  case FlushRX:
    spi_write(tx_buffer, length);
    break;
  case ReadRegister:
  case RRxPayload:
    spi_read(tx_buffer, length, buffer);
  default:
    break;
  }
  // asm("bkpt");

  // spi_write(tx_buffer, length, (transferType != RRxPayload) ? rx_buffer : buffer);
  return rx_buffer;
}

char get_nrf_register(Nrf24l01RegisterNames_t reg_type)
{
  transfer(reg_type, sizeof(char), ReadRegister, rx_buffer);

  return rx_buffer[0];
}

void get_nrf_register_long(Nrf24l01RegisterNames_t reg_type, char* register_long)
{
  transfer(reg_type, 5 * sizeof(char), ReadRegister, register_long);
}

// void get_nrf_register_long(Nrf24l01RegisterNames_t reg_type, char* register_long)
// {
//     transfer_read(reg_type, sizeof(unsigned int), ReadRegister);

//     for (unsigned int i = 0; i <  sizeof(unsigned int); i++)
//     {
//         register_long[i] = rx_buffer[i+1];
//     }
// }

void set_nrf_register_long(Nrf24l01RegisterNames_t reg_type, char* payload)
{
  transfer(reg_type, 5, WriteRegister, payload);
}

// void set_nrf_register_long(Nrf24l01RegisterNames_t reg_type, char* payload)
// {
//     transfer_write(reg_type, sizeof(unsigned int), WriteRegister, payload);
// }

void set_bit_nrf_register(Nrf24l01RegisterNames_t reg_type, char bit_position)
{
  char current_value = get_nrf_register(reg_type);
  char new_value = current_value | (1 << bit_position);
  transfer(reg_type, 1, WriteRegister, &new_value);
}

void clear_bit_nrf_register(Nrf24l01RegisterNames_t reg_type, char bit_position)
{
  char current_value = get_nrf_register(reg_type);
  char new_value = current_value & ~(1 << bit_position);
  transfer(reg_type, 1, WriteRegister, &new_value);
}

void replace_nrf_register(Nrf24l01RegisterNames_t reg_type, char new_value)
{
  transfer(reg_type, 1, WriteRegister, &new_value);
}

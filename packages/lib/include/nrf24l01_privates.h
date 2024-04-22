#ifndef NRF24L01_PRIVATES_H
#define NRF24L01_PRIVATES_H

enum
{
  TX_BUFFER_SIZE = 32,
  RX_BUFFER_SIZE = sizeof(Nrf24l01Registers_t)
};

extern char rx_buffer[RX_BUFFER_SIZE];

enum
{
  PackageBufferSize = 32
};

enum
{
  R_RX_PAYLOAD = 0x61,
  W_TX_PAYLOAD = 0xA0,
  FLUSH_TX = 0xE1,
  FLUSH_RX = 0xE2
};

enum
{
  readTimerFunctionPtr = 0,
  crcResetFunctionPtr = 1,
  crcLoadFunctionPtr = 2,
  crcReadFunctionPtr = 3,
};

typedef enum transferType
{
  ReadRegister,
  WriteRegister,
  RRxPayload,
  WTxPayload,
  FlushTX,
  FlushRX
} TransferType_t;

static inline __attribute__((always_inline)) void configure_package_type(char target_register, TransferType_t transfer_type, char* tx_buffer)
{
  switch (transfer_type)
  {
  case ReadRegister:
    tx_buffer[0] = target_register & ~(0b111 << 5);
    break;
  case WriteRegister:
    tx_buffer[0] = ((char) (1 << 5) | target_register) & ~(0b11 << 6);
    break;
  case RRxPayload:
    tx_buffer[0] = R_RX_PAYLOAD;
    break;
  case WTxPayload:
    tx_buffer[0] = W_TX_PAYLOAD;
    break;
  case FlushTX:
    tx_buffer[0] = FLUSH_TX;
    break;
  case FlushRX:
    tx_buffer[0] = FLUSH_RX;
    break;
  default:
    return;
  }

}

void set_bit_nrf_register(Nrf24l01RegisterNames_t, char);
void clear_bit_nrf_register(Nrf24l01RegisterNames_t, char);
void replace_nrf_register(Nrf24l01RegisterNames_t, char);
void set_nrf_register_long(Nrf24l01RegisterNames_t, char*);
void transfer_read(char target_register, unsigned int length, TransferType_t t);
void transfer_read_wbuffer(char target_register, unsigned int length, TransferType_t t, char *custom_buffer);
void transfer_write(char target_register, unsigned int length, TransferType_t t, char* payload);
void unset_ce(void);
void set_ce(void);
void get_nrf_register_long(Nrf24l01RegisterNames_t, char*);
#endif
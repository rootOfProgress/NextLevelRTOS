#ifndef NRF24L01_H
#define NRF24L01_H

#define W_REGISTER(target_reg) (char) ((char) (1 << 5) | (char) target_reg)
#define R_REGISTER(target_reg) (char) ((char) (0 << 5) | (char) target_reg)




typedef enum transferType { 
    ReadRegister,
    WriteRegister,
    RRxPayload,
    WTxPayload,
    FlushTX
} TransferType_t;

typedef enum OperatingMode {
    SLAVE,
    MASTER
} OperatingMode_t;

void spin(int);
void power_on();

#define R_RX_PAYLOAD 0x61 
#define W_TX_PAYLOAD 0xA0 
#define FLUSH_TX 0xE1

typedef struct Nrf24l01Registers {
    char config;
    char en_aa;
    char en_rxaddr;
    char setup_aw;
    char setup_retr;
    char rf_ch; // 0x5
    char rf_setup; // 0x6
    char status;
    char observe_tx;
    char rpd;
    char rx_addr_p0[8];
    char rx_addr_p1[8];
    char rx_addr_p2;
    char rx_addr_p3;
    char rx_addr_p4;
    char rx_addr_p5;
    char tx_addr[8];
    char rx_pw_p0;
    char rx_pw_p1;
    char rx_pw_p2;
    char rx_pw_p3;
    char rx_pw_p4;
    char rx_pw_p5;
    char fifo_status; // 0x17
    char dyndp;
    char feature;
} Nrf24l01Registers_t;

enum {
    TX_BUFFER_SIZE = 32,
    RX_BUFFER_SIZE = sizeof(Nrf24l01Registers_t)
};

extern char receive_buffer[RX_BUFFER_SIZE];
extern char tx_buffer[TX_BUFFER_SIZE];

extern Nrf24l01Registers_t nrf24l01_regs;

typedef enum Nrf24l01RegisterNames {
    CONFIG = 0x0,
    EN_AA,
    EN_RXADDR,
    SETUP_AW,
    SETUP_RETR,
    RF_CH, // 0x5
    RF_SETUP, // 0x6
    STATUS,
    OBSERVE_TX,
    RPD,
    RX_ADDR_P0,
    RX_ADDR_P1,
    RX_ADDR_P2,
    RX_ADDR_P3,
    RX_ADDR_P4,
    RX_ADDR_P5,
    TX_ADDR,
    RX_PW_P0,
    RX_PW_P1,
    RX_PW_P2,
    RX_PW_P3,
    RX_PW_P4,
    RX_PW_P5,
    FIFO_STATUS, // 0x17
    DYNDP = 0x1C,
    FEATURE = 0x1D
} Nrf24l01RegisterNames_t;

void set_bit_nrf_register(Nrf24l01RegisterNames_t, char);
void clear_bit_nrf_register(Nrf24l01RegisterNames_t, char);
char get_nrf_register(Nrf24l01RegisterNames_t);
void get_nrf_register_long(Nrf24l01RegisterNames_t, char*);
void replace_nrf_register(Nrf24l01RegisterNames_t, char);
void transfer(char, char*, unsigned int, TransferType_t); 
void set_nrf_register_long(Nrf24l01RegisterNames_t, char*);



#endif
#ifndef NRF24L01_H
#define NRF24L01_H

#define W_REGISTER(target_reg) (char) ((char) (1 << 5) | (char) target_reg)
#define R_REGISTER(target_reg) (char) ((char) (0 << 5) | (char) target_reg)

typedef enum transferType { 
    read_register,
    write_register,
    r_rx_payload,
    w_tx_payload,
} transferType_t;

void spin(int);
void power_on();

#define R_RX_PAYLOAD 0x61 
#define W_TX_PAYLOAD 0xA0 
#define FLUSH_TX 0xE1

//#define FIFO_STATUS 0x17
typedef struct nrf24l01_registers {
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
    char rx_addr_p0[5];
    char rx_addr_p1[5];
    char rx_addr_p2;
    char rx_addr_p3;
    char rx_addr_p4;
    char rx_addr_p5;
    char tx_addr[5];
    char rx_pw_p0;
    char rx_pw_p1;
    char rx_pw_p2;
    char rx_pw_p3;
    char rx_pw_p4;
    char rx_pw_p5;
    char fifo_status; // 0x17
    char dyndp;
    char feature;
} nrf24l01_registers_t;


typedef enum nrf24l01_registermap {
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
} nrf24l01_registermap_t;



#endif
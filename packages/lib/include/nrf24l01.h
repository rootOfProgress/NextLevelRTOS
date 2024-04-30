#ifndef NRF24L01_H
#define NRF24L01_H
// #include "nrf24l01_privates.h"
#define W_REGISTER(target_reg) (char) ((char) (1 << 5) | (char) target_reg)
#define R_REGISTER(target_reg) (char) ((char) (0 << 5) | (char) target_reg)

void spin(int);
void power_on();

enum
{
  PRIM_RX = 0,
  PWR_UP = 1,
  CRCO = 2,
  EN_CRC = 3,
  MASK_MAX_RT = 4,
  MASK_TX_DS = 5,
  MASK_RX_DR = 6
};

typedef struct TxObserve
{
  unsigned int totalLostPackages;
  unsigned int retransmitCount;
  unsigned int maxRetransmits;
  unsigned int timeUntilAckArrived;
  unsigned int totalElapsed;
  unsigned int bytesSend;
  unsigned int totalRetransmits;
  unsigned int signalStrength;
  unsigned int totalPackages;
} TxObserve_t;

typedef struct TxConfig
{
  unsigned int autoRetransmitDelay;
  unsigned int retransmitCount;
} TxConfig_t;

typedef struct
{
  // CONFIG Register
  union
  {
    struct
    {
      char reserved1 : 1;
      char MASK_RX_DR : 1;
      char MASK_TX_DS : 1;
      char MASK_MAX_RT : 1;
      char EN_CRC : 1;
      char CRCO : 1;
      char PWR_UP : 1;
      char PRIM_RX : 1;
    } config;
    char config_raw;
  };

  // EN_AA Register
  union
  {
    struct
    {
      char reserved2 : 2;
      char ENAA_P5 : 1;
      char ENAA_P4 : 1;
      char ENAA_P3 : 1;
      char ENAA_P2 : 1;
      char ENAA_P1 : 1;
      char ENAA_P0 : 1;
    } en_aa;
    char en_aa_raw;
  };

  // EN_RXADDR Register
  union
  {
    struct
    {
      char reserved3 : 2;
      char ERX_P5 : 1;
      char ERX_P4 : 1;
      char ERX_P3 : 1;
      char ERX_P2 : 1;
      char ERX_P1 : 1;
      char ERX_P0 : 1;
    } en_rxaddr;
    char en_rxaddr_raw;
  };

  // SETUP_AW Register
  union
  {
    struct
    {
      char reserved4 : 6;
      char AW : 2;
    } setup_aw;
    char setup_aw_raw;
  };

  // SETUP_RETR Register
  union
  {
    struct
    {
      char ARD : 4;
      char ARC : 4;
    } setup_retr;
    char setup_retr_raw;
  };

  // RF_CH Register
  union
  {
    struct
    {
      char reserved5 : 1;
      char RF_CH : 7;
    } rf_ch;
    char rf_ch_raw;
  };

  // RF_SETUP Register
  union
  {
    struct
    {
      char CONT_WAVE : 1;
      char RF_PWR : 2;
      char RF_DR_HIGH : 1;
      char PLL_LOCK : 1;
      char RF_DR_LOW : 1;
      char reserved6 : 2;
    } rf_setup;
    char rf_setup_raw;
  };

  // STATUS Register
  union
  {
    struct
    {
      char reserved7 : 1;
      char RX_DR : 1;
      char TX_DS : 1;
      char MAX_RT : 1;
      char RX_P_NO : 3;
      char TX_FULL : 1;
    } status;
    char STATUS_raw;
  };

  // OBSERVE_TX Register
  union
  {
    struct
    {
      char PLOS_CNT : 4;
      char ARC_CNT : 4;
    };
    char OBSERVE_TX_raw;
  };

  // RPD Register
  union
  {
    struct
    {
      char reserved8 : 6;
      char RPD : 1;
    };
    char RPD_raw;
  };

  // RX_ADDR_P0 Register
  union
  {
    struct
    {
      char RX_ADDR_P0[5]; // 40 bits (5 bytes)
    };
    char RX_ADDR_P0_raw[5];
  };

  // RX_ADDR_P1 Register
  union
  {
    struct
    {
      char RX_ADDR_P1[5]; // 40 bits (5 bytes)
    };
    char RX_ADDR_P1_raw[5];
  };

  // RX_ADDR_P2 Register
  union
  {
    struct
    {
      char RX_ADDR_P2 : 8; // 8 bits
    };
    char RX_ADDR_P2_raw;
  };

  // RX_ADDR_P3 Register
  union
  {
    struct
    {
      char RX_ADDR_P3 : 8; // 8 bits
    };
    char RX_ADDR_P3_raw;
  };

  // RX_ADDR_P4 Register
  union
  {
    struct
    {
      char RX_ADDR_P4 : 8; // 8 bits
    };
    char RX_ADDR_P4_raw;
  };

  // RX_ADDR_P5 Register
  union
  {
    struct
    {
      char RX_ADDR_P5 : 8; // 8 bits
    };
    char RX_ADDR_P5_raw;
  };

  // TX_ADDR Register
  union
  {
    struct
    {
      char TX_ADDR[5]; // 40 bits (5 bytes)
    };
    char TX_ADDR_raw[5];
  };

  // RX_PW_P0 Register
  union
  {
    struct
    {
      char reserved9 : 2;
      char RX_PW_P0 : 6;
    };
    char RX_PW_P0_raw;
  };

  // RX_PW_P1 Register
  union
  {
    struct
    {
      char reserved10 : 2;
      char RX_PW_P1 : 6;
    };
    char RX_PW_P1_raw;
  };

  // RX_PW_P2 Register
  union
  {
    struct
    {
      char reserved11 : 2;
      char RX_PW_P2 : 6;
    };
    char RX_PW_P2_raw;
  };

  // RX_PW_P3 Register
  union
  {
    struct
    {
      char reserved12 : 2;
      char RX_PW_P3 : 6;
    };
    char RX_PW_P3_raw;
  };

  // RX_PW_P4 Register
  union
  {
    struct
    {
      char reserved13 : 2;
      char RX_PW_P4 : 6;
    };
    char RX_PW_P4_raw;
  };

  // RX_PW_P5 Register
  union
  {
    struct
    {
      char reserved14 : 2;
      char RX_PW_P5 : 6;
    };
    char RX_PW_P5_raw;
  };

  // FIFO_STATUS Register
  union
  {
    struct
    {
      char reserved15 : 1;
      char TX_REUSE : 1;
      char TX_FIFO_FULL : 1;
      char TX_EMPTY : 1;
      char reserved16 : 2;
      char RX_FULL : 1;
      char RX_EMPTY : 1;
    };
    char FIFO_STATUS_raw;
  };

  // DYNPD Register
  union
  {
    struct
    {
      char reserved17 : 2;
      char DPL_P5 : 1;
      char DPL_P4 : 1;
      char DPL_P3 : 1;
      char DPL_P2 : 1;
      char DPL_P1 : 1;
      char DPL_P0 : 1;
    };
    char DYNPD_raw;
  };

  // FEATURE Register
  union
  {
    struct
    {
      char reserved18 : 5;
      char EN_DPL : 1;
      char EN_ACK_PAY : 1;
      char EN_DYN_ACK : 1;
    };
    char FEATURE_raw;
  };
} NRF24L01_Registers_Full;

/* --------------------------------------------------- */
/* -------------------Driver Specifics---------------- */
/* --------------------------------------------------- */
typedef struct Nrf24l01Registers
{
  char config;
  char en_aa;
  char en_rxaddr;
  char setup_aw;
  char setup_retr;
  char rf_ch; // 0x5
  char rf_setup; // 0x6
  char status;
  // NRF2401_Status_t status;
  char observe_tx;
  char rpd;
  char rx_addr_p0[8];
  char rx_addr_p1[8];
  // unsigned int rx_addr_p0;
  // unsigned int rx_addr_p1;
  char rx_addr_p2;
  char rx_addr_p3;
  char rx_addr_p4;
  char rx_addr_p5;
  char tx_addr[8];
  // unsigned int tx_addr;
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

typedef enum OperatingMode
{
  SLAVE,
  MASTER
} OperatingMode_t;

extern Nrf24l01Registers_t nrf24l01_regs;

typedef enum Nrf24l01RegisterNames
{
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

// char get_nrf_status(void);
char get_nrf_rpd(void);
void clear_ir_maxrt_flag(void);
void start_listening(void);
void stop_listening(void);
void nrf_flush_rx(void);
void nrf_receive_payload(unsigned int, char*);
void clear_rx_dr_flag(void);
void clear_tx_ds_flag(void);
void nrf_power_off(void);
void nrf_power_on(void);
char configure_device(Nrf24l01Registers_t*, OperatingMode_t);
void get_nrf_config(Nrf24l01Registers_t*);
unsigned int transmit_single_package(void);
unsigned int transmit_all_packages(void);
// char check_for_received_data(Nrf24l01Registers_t* config, char* response_buffer);
unsigned int tx_ack_receive_isr(Nrf24l01Registers_t *nrf_registers);
void transmit_with_autoack(TxConfig_t *tx_config,
                           char *receivedAckPackage,
                           char *outBuffer);
TxObserve_t get_current_tx_state(void);
void append_os_core_function(unsigned int (*function_ptr)());

/* --------------------------------------------------- */
/* -------------------Custom Extensions--------------- */
/* --------------------------------------------------- */
typedef struct ReceiveHeader
{
  char node_number;
  char package_type;
} ReceiveHeader_t;

/** @struct ReceiveBodyOSRequest
 *  @brief This structure blah blah blah...
 *  @var ReceiveBodyOSRequest::header
 *  Obligatory package information
 *  @var ReceiveBodyOSRequest::os_command
 *  Same format as uart rx payload, 4 Byte i.e. <State><Payload_0><Payload_1><Payload_2>
 */
typedef struct ReceiveBodyOSRequest
{
  ReceiveHeader_t header;
  char os_command;
} ReceiveBodyOSRequest_t;

typedef struct ReceiveBodyGeneralRequest
{
  ReceiveHeader_t header;
  char package_size;
  char* payload;
} ReceiveBodyGeneralRequest_t;

typedef enum ReceiveRequestTypes
{
  OsRequest = 0x00,
  GeneralRequest,
} ReceiveRequestTypes_t;

/**
 * @brief Enables CRC (Cyclic Redundancy Check).
 *
 * This function sets the CRC bit in the CONFIG register to enable CRC functionality.
 * CRC is a method used for error detection in data transmission or storage.
 *
 * @return The value of the CRC bit in the CONFIG register after enabling CRC.
 */
unsigned int enable_crc(void);

/**
 * @brief Disables CRC (Cyclic Redundancy Check).
 *
 * This function clears the CRC bit in the CONFIG register to disable CRC functionality.
 * CRC is a method used for error detection in data transmission or storage.
 *
 * @return The value of the CRC bit in the CONFIG register after disabling CRC.
 */
unsigned int disable_crc(void);

unsigned int load_tx_buffer(unsigned int length, char* payload);

void enable_rx_and_listen(void);
void disable_rx(void);
void nrf_flush_tx(void);

char get_nrf_register(Nrf24l01RegisterNames_t);

static inline __attribute__((always_inline)) char get_nrf_fifo(void);
static inline __attribute__((always_inline)) char get_nrf_status(void);
static inline __attribute__((always_inline)) char check_for_received_data(Nrf24l01Registers_t* config, char* response_buffer);
static inline __attribute__((always_inline)) char get_nrf_fifo(void)
{
  return get_nrf_register(FIFO_STATUS);
}

static inline __attribute__((always_inline)) char get_nrf_status(void)
{
  return get_nrf_register(STATUS);
}

static inline __attribute__((always_inline)) char check_for_received_data(Nrf24l01Registers_t* config, char* response_buffer)
{
  char ret = 1;
  int pipe = 7;
  pipe = (get_nrf_status() >> 1) & 0x7;
  if (pipe >= 0 && pipe <= 5)
  {
    stop_listening();
    switch (pipe)
    {
    case 0:
      nrf_receive_payload(config->rx_pw_p0, response_buffer);
      break;
    case 1:
      nrf_receive_payload(config->rx_pw_p1, response_buffer);
      break;
    case 2:
      nrf_receive_payload(config->rx_pw_p2, response_buffer);
      break;
    case 3:
      nrf_receive_payload(config->rx_pw_p3, response_buffer);
      break;
    case 4:
      nrf_receive_payload(config->rx_pw_p4, response_buffer);
      break;
    case 5:
      nrf_receive_payload(config->rx_pw_p5, response_buffer);
      break;
    default:
      ret = 0;
      break;
    }
    start_listening();
  }
  else
  {
    ret = 0;
  }
  return ret;
}
#endif
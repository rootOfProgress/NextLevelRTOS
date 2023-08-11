#ifndef REMOTE_H
#define REMOTE_H


typedef enum Commands 
{
    AdjustSpeed = 0,

} Commands_t;

/*
 *
 * 
 *
 *
 */
typedef struct RemoteCmd
{
    unsigned int cmd_type;
    unsigned int cmd_argument;
} RemoteCmd_t;      

typedef struct CrcAppendix
{
    unsigned int checksum;
} CrcAppendix_t;

// typedef struct TxPayload
// {
//     RemoteCmd_t cmd;
//     CrcAppendix_t crc;
// } TxPayload_t;

typedef struct TxPayload
{
    unsigned int cmd_type;
    unsigned int cmd_argument;
    unsigned int checksum;
    // unsigned int bla;
} TxPayload_t;


void remote_send_command(TxPayload_t* tx_payload);
void remote_prepare_payload(RemoteCmd_t* cmd, CrcAppendix_t* crc);

#endif
#ifndef DMA_H
#define DMA_H

#include "lang.h"

// ISR dma2 stream 5 specifics
enum {
    DmaHisrTeif5 = 9, // transfer error
    DmaHisrTcif5 = 11, // transfer complete
};

enum { Dma1BaseAdress = 0x40026000, Dma2BaseAdress = 0x40026400 };


#define USART1_DR 0x40011004
#define USART1_cr1 0x4001100C
#define USART1_cr3 0x40011014

typedef struct DmaStreamRegisterMap {
    unsigned int dma_sXcr;
    unsigned int dma_sXndtr;
    unsigned int dma_sXpar;
    unsigned int dma_sXm0ar;
    unsigned int dma_sXm1ar;
    unsigned int dma_sXfcr;
} DmaStreamRegisterMap_t;

typedef struct DmaCommonRegisterMap {
    unsigned int dma_lisr;
    unsigned int dma_hisr;
    unsigned int dma_lifcr;
    unsigned int dma_hifcr;
} DmaCommonRegisterMap_t;

// general, p 171:
// Channel 4:
// Stream_5 - USART1_RX
// Stream_7 - USART1_TX

enum {
    dmaCrTcie = 4,
    dmaCrDir = 6,
    dmaCrMinc = 10,
    dmaCrChsel = 25
};

typedef enum DmaRccSpecifics {
    DMA1EN = 21, // on AHB1 Bus
    DMA2EN = 22 // on AHB1 Bus
} DmaRccSpecifics_t;

typedef enum DmaModes {
    PeripherialToMemory = 0,
    MemoryToPeripherial,
    MemoryToMemory
} DmaModes_t;

// used for IR Handling later
typedef enum DmaJobType {
    DmaIsIdle = 0,
    DmaTransferedExternalTask = 1 << 0,
    DmaHadTransferError = 1 << 1,
    DmaWaitsForExternalTask = 1 << 3
} DmaJobType_t;

typedef struct DmaTransferSpecifics {
    unsigned int msize : 2, dir : 2, tcie : 1, htie : 1, minc : 1, pinc: 1, chsel : 3, ndtr : 16, stream_number : 4,: 1;
    unsigned int source_address;
    unsigned int destination_address;
    DmaJobType_t dma_job_type;
} DmaTransferSpecifics_t;


extern DmaTransferSpecifics_t dt;
extern DmaJobType_t dma_interrupt_action;

static inline __attribute__((always_inline)) unsigned int get_stream_offset(unsigned int stream_no)
{
    return 0x10 + (stream_no * 0x18);
}

void dma_init(void);

static inline __attribute__((always_inline)) void dma_transfer(
    DmaTransferSpecifics_t *dma_transfer_config,
    DmaModes_t dma_mode)
{
    DmaStreamRegisterMap_t *dma_streamX_register = (DmaStreamRegisterMap_t*) (Dma2BaseAdress + get_stream_offset(dma_transfer_config->stream_number));
    // disable device
    unsigned int crx_result;
    if ((crx_result = READ_REGISTER(&dma_streamX_register->dma_sXcr)) & 1)
        WRITE_REGISTER(&dma_streamX_register->dma_sXcr, crx_result & ~1);    

    switch (dma_mode)
    {
    case PeripherialToMemory:
        WRITE_REGISTER(&dma_streamX_register->dma_sXpar, dma_transfer_config->source_address);
        WRITE_REGISTER(&dma_streamX_register->dma_sXm0ar, dma_transfer_config->destination_address);
        
        // only if uart transfer!!
        // DMAR
        WRITE_REGISTER(USART1_cr3, READ_REGISTER(USART1_cr3) | 1 << 6);
        // ir disable
        WRITE_REGISTER(USART1_cr1, READ_REGISTER(USART1_cr1) & ~(1 << 5));
        break;
    case MemoryToPeripherial:
        WRITE_REGISTER(&dma_streamX_register->dma_sXpar, dma_transfer_config->source_address);
        WRITE_REGISTER(&dma_streamX_register->dma_sXm0ar, dma_transfer_config->destination_address);
        break;
    default:
        return;
        break;
    }
    WRITE_REGISTER(&dma_streamX_register->dma_sXndtr, dma_transfer_config->ndtr);

    WRITE_REGISTER(&dma_streamX_register->dma_sXcr, 
        dma_mode << dmaCrDir | 
        (dma_transfer_config->chsel << dmaCrChsel) | 
        (dma_transfer_config->minc << dmaCrMinc) |
        (dma_transfer_config->tcie << dmaCrTcie)
    );
    unsigned int dma_sXcr = READ_REGISTER(&dma_streamX_register->dma_sXcr);
    WRITE_REGISTER(&dma_streamX_register->dma_sXcr, dma_sXcr | 1);
}


#endif
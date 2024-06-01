#ifndef DMA_H
#define DMA_H

#include "lang.h"
#include "dma_types.h"

// ISR dma2 stream 5 specifics
enum
{
  DmaHisrTeif5 = 9, // transfer error
  DmaHisrTcif5 = 11, // transfer complete
};

enum { Dma1BaseAdress = 0x40026000, Dma2BaseAdress = 0x40026400 };

#define USART1_DR 0x40011004
#define USART1_cr1 0x4001100C
#define USART1_cr3 0x40011014

typedef struct DmaStreamRegisterMap
{
  unsigned int dma_sXcr;
  unsigned int dma_sXndtr;
  unsigned int dma_sXpar;
  unsigned int dma_sXm0ar;
  unsigned int dma_sXm1ar;
  unsigned int dma_sXfcr;
} DmaStreamRegisterMap_t;

typedef struct DmaCommonRegisterMap
{
  unsigned int dma_lisr;
  unsigned int dma_hisr;
  unsigned int dma_lifcr;
  unsigned int dma_hifcr;
} DmaCommonRegisterMap_t;

// general, p 171:
// Channel 4:
// Stream_5 - USART1_RX
// Stream_7 - USART1_TX

enum
{
  dmaCrTcie = 4,
  dmaCrDir = 6,
  dmaCrMinc = 10,
  dmaCrChsel = 25
};

typedef enum DmaRccSpecifics
{
  DMA1EN = 21, // on AHB1 Bus
  DMA2EN = 22 // on AHB1 Bus
} DmaRccSpecifics_t;

typedef enum DmaModes
{
  PeripherialToMemory = 0,
  MemoryToPeripherial,
  MemoryToMemory
} DmaModes_t;

// used for IR Handling later
typedef enum DmaJobType
{
  DmaIsIdle = 0,
  DmaTransferedExternalTask = 1 << 0,
  DmaHadTransferError = 1 << 1,
  DmaWaitsForExternalTask = 1 << 3
} DmaJobType_t;

typedef struct DmaTransferSpecifics
{
  unsigned int msize : 2, dir : 2, tcie : 1, htie : 1, minc : 1, pinc: 1, chsel : 3, ndtr : 16, stream_number : 4, : 1;
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
void dma_transfer(DmaTransferSpecifics_t *dma_transfer_config, DmaModes_t dma_mode, SourcePeripherial_t source);

#endif
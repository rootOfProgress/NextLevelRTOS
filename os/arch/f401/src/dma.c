#include "dma.h"
#include "rcc.h"
#include "lang.h"
#include "uart.h"
#include "dma_types.h"

DmaJobType_t dma_interrupt_action;

void dma_init(void)
{
  dma_interrupt_action = DmaIsIdle;
  RccRegisterMap_t* rcc_registers = (RccRegisterMap_t*) RccBaseAdress;
  WRITE_REGISTER(&rcc_registers->ahb1enr, READ_REGISTER(&rcc_registers->ahb1enr) | (1 << DMA2EN));
}

void dma_transfer(
  DmaTransferSpecifics_t *dma_transfer_config,
  DmaModes_t dma_mode,
  SourcePeripherial_t source)
{
  DmaStreamRegisterMap_t *dma_streamX_register = (DmaStreamRegisterMap_t*) (Dma2BaseAdress + get_stream_offset(dma_transfer_config->stream_number));
  // disable device
  unsigned int crx_result;
  if ((crx_result = READ_REGISTER(&dma_streamX_register->dma_sXcr)) & 1)
    WRITE_REGISTER(&dma_streamX_register->dma_sXcr, crx_result & ~1);

  switch (dma_mode)
  {
  case PeripherialToMemory:

    switch (source)
    {
    case UART:
      dma_transfer_config->source_address = (unsigned int) & ((UartRegisterMap_t*) Usart1Baseadress)->dr;
      break;
    default:
      break;
    }

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

void dma2_stream5_ir_handler(void)
{
  // direct access for performance reasons
  unsigned int dma_hisr = READ_REGISTER(Dma2BaseAdress | 0x04);

  if (dma_hisr & (1 << DmaHisrTeif5))
  {
    dma_interrupt_action = DmaHadTransferError;
  }
  else if (dma_hisr & (1 << DmaHisrTcif5))
  {
    switch (dma_interrupt_action)
    {
    case DmaWaitsForExternalTask:
      dma_interrupt_action &= ~DmaWaitsForExternalTask;
      dma_interrupt_action |= DmaTransferedExternalTask;
      // notify task_create

      // enable usart interrupt
      break;

    default:
      break;
    }
  }
  // clear all stream 5 specifics
  WRITE_REGISTER(Dma2BaseAdress | 0x0C, READ_REGISTER(Dma2BaseAdress | 0x0C) | (0xF << 8));
  // dma disable
  WRITE_REGISTER(USART1_cr3, READ_REGISTER(USART1_cr3) & ~(1 << 6));
  // enable usart ir again
  WRITE_REGISTER(USART1_cr1, READ_REGISTER(USART1_cr1) | (1 << 5));
}
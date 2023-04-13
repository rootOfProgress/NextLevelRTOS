#include "dma.h"
#include "rcc.h"
#include "lang.h"

DmaJobType_t dma_interrupt_action;

void dma_init(void)
{
    dma_interrupt_action = DmaIsIdle;
    RccRegisterMap_t* rcc_registers = (RccRegisterMap_t*) RccBaseAdress;
    WRITE_REGISTER(&rcc_registers->ahb1enr, READ_REGISTER(&rcc_registers->ahb1enr) | (1 << DMA2EN));
}

// dummy 
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
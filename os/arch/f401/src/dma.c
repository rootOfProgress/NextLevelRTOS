#include "dma.h"
#include "rcc.h"
#include "lang.h"

void dma_init(void)
{
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
        case DmaReceivesExternalTask:
            dma_interrupt_action = DmaIsIdle;
            // notify task_create
            
            // enable usart interrupt 
            break;
        
        default:
            break;
        }
    }
    // clear all stream 5 specifics
    WRITE_REGISTER(Dma2BaseAdress | 0x0C, READ_REGISTER(Dma2BaseAdress | 0x0C) | (0xF << 8));
    WRITE_REGISTER(USART1_cr3, READ_REGISTER(USART1_cr3) & ~(1 << 6));
    // ir disable
    WRITE_REGISTER(USART1_cr1, READ_REGISTER(USART1_cr1) | (1 << 5));
    // enable usart ir again
}
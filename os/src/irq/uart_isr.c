#include "uart.h"
#include "uart_common.h"
#include "data/list.h"
#include "memory.h"
#include "exception.h"
#include "process/scheduler.h"
#include "panic.h"
#include "dma.h"

UartStates_t state;
TransferInfo_t transfer_list[MAX_WAITING_TRANSFERS];
TaskInformation_t tInfo;

unsigned int in_buffer;
unsigned int byte_in_id;

volatile unsigned int num_of_waiting_transfers;
unsigned char bytes_received;
unsigned char uart_rx_buffer[BUFFERSIZE];

void init_transfer_handler(void)
{
    num_of_waiting_transfers = 0;
    
    for (unsigned int i = 0; i < MAX_WAITING_TRANSFERS; i++)
    {
        transfer_list[i].length = 0;
        transfer_list[i].start_adress = 0;
    }
}

void __attribute__((optimize("O0"))) setup_transfer(char* address, unsigned int length)
{
    if (num_of_waiting_transfers == MAX_WAITING_TRANSFERS)
        return;

    if (num_of_waiting_transfers == 0)
        wakeup_pid(kernel_pids.transfer_handler);
    
    transfer_list[num_of_waiting_transfers].start_adress = address;
    transfer_list[num_of_waiting_transfers].length = length;
    num_of_waiting_transfers++;
}

void __attribute__((optimize("O0"))) transfer_handler(void)
{
    init_transfer_handler();
    
    while (1)
    {
        if (num_of_waiting_transfers)
        {
            print(transfer_list[num_of_waiting_transfers - 1].start_adress, transfer_list[num_of_waiting_transfers - 1].length);
            num_of_waiting_transfers--;
        }
        block_current_task();
        // SV_YIELD_TASK;
        // @todo : wake up task on DMA finish
    }    
}

void init_isr(void)
{
    byte_in_id = 4;
    state = RX_READY;
    in_buffer = 0;
    bytes_received = 0;
    for (unsigned int i = 0; i < BUFFERSIZE; i++)
        uart_rx_buffer[i] = 0;
}

void __attribute__((interrupt))  __attribute__((optimize("O0"))) uart_isr_handler(void)
{
    if (READ_REGISTER(USART1_sr) & (1 << 3))
    {
        read_data_register();
        return;
    }

    uart_rx_buffer[bytes_received++] = read_data_register();
    if (bytes_received != BUFFERSIZE)
        return;
    switch (state)
    {
    case RX_READY:
        if ((*((unsigned int*)uart_rx_buffer) >> 8) == MAGIC)
        {
            state = *((unsigned int*)uart_rx_buffer) & 0xFF;
        }
        break;
    case PREPARE_TASK_TRANSFER:        
        tInfo.task_size = (unsigned int) *((unsigned int*) uart_rx_buffer); 
        tInfo.start_adress = allocate(tInfo.task_size); 
        
        if (!tInfo.start_adress)
            invoke_panic(OUT_OF_MEMORY);

        // notify host to recompile with correct offset
        setup_transfer((char*) &tInfo.start_adress, 4);

        DmaTransferSpecifics_t dt;
    
        dt.chsel = 4;
        dt.minc = 1;
        // dt.ndtr = 0x200;
        dt.ndtr = tInfo.task_size;

        // uart rx
        dt.source_address = (unsigned int) &((UartRegisterMap_t*) Usart1Baseadress)->dr;
        // dt.source_address = 0x40011004;
        dt.destination_address = (unsigned int) tInfo.start_adress;
        dt.stream_number = 5;
        dt.tcie = 1;    
        dt.dma_job_type = DmaWaitsForExternalTask;
        dma_interrupt_action = DmaWaitsForExternalTask;
        dma_transfer(&dt, PeripherialToMemory);
        state = RX_READY;
        break;
    case REQUEST_STATISTIC:
        wakeup_pid(kernel_pids.statistic_manager);
        state = RX_READY;
        break;
    case REBOOT:
        state = RX_READY;
        soft_reset();
        break;
    case REQUEST_TEST_RESULT:
        char *test = "DDDDEEEE";
        print(test, 8);
        print((char*) 0x20000000, 4);
        WRITE_REGISTER(0x20000000, 0);
        state = RX_READY;
        return;
    default:
        break;
    }
    bytes_received = 0;

}
#include "uart.h"
#include "uart_common.h"
#include "data/list.h"
#include "memory.h"
#include "exception.h"
#include "process/scheduler.h"
#include "panic.h"
#include "dma.h"

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
    *internal_rx_state = RX_READY;
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
        
    *internal_rx_state = *((unsigned int*)uart_rx_buffer) & 0xFF;
    wakeup_pid(kernel_pids.external_io_runner);
    bytes_received = 0;
}
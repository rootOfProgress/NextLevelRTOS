#include "devices/uart.h"
#include "data/list.h"
#include "memory.h"
#include "exception.h"
#include "process/scheduler.h"
#include "panic.h"

UartStates_t state;
TaskInformation_t *tInfo = NULL;

List_t *transfer_list = NULL;
unsigned int in_buffer = 0;
unsigned int byte_in_id;
unsigned int bufferIndex;
char buffer[16];
char *p;

void init_transfer_handler(void)
{
    transfer_list = (List_t*) new_list();
}

void __attribute__((optimize("O0"))) setup_transfer(char* address, unsigned int length)
{
    if (transfer_list->size > 10)
        return;

    if (transfer_list->size == 0)
        wakeup_pid(0);
        //unblock_task(0);


    TransferInfo_t* t = (TransferInfo_t*) allocate(sizeof(TransferInfo_t));
    
    if (!t)
        invoke_panic(OUT_OF_MEMORY);
    
    t->start_adress = address;
    t->length = length;
    push(transfer_list, (void*) t);
}

void __attribute__((optimize("O0"))) transfer_handler(void)
{
    SingleLinkedNode_t* node;
    init_transfer_handler();
    while (1)
    {
        if ((node = pop(transfer_list)))
        {
            TransferInfo_t* transfer = (TransferInfo_t*) node->data;
            print(transfer->start_adress, transfer->length);
            if (deallocate((unsigned int*) transfer) == 0)
                invoke_panic(MEMORY_DEALLOC_FAILED);
            if (deallocate((unsigned int*) node) == 0)
                invoke_panic(MEMORY_DEALLOC_FAILED);
        }
        else
        {
            block_current_task();
         
            // transfer_list->size = -1;
            // SV_YIELD_TASK_BLOCK;
        }
        SV_YIELD_TASK;
    }
    
}

void init_isr(void)
{
    byte_in_id = 4;
    state = RX_READY;
    bufferIndex = 0;
    for (unsigned int i = 0; i < 16; i++)
        buffer[i] = 0;
}

void __attribute__((interrupt)) uart_isr_handler(void)
{
    switch (state)
    {
    case RX_READY:
        unsigned int content = read_data_register();
        in_buffer |= content << ((--byte_in_id) * 8);
        if ((in_buffer & 0xFFFFFF) == magic)
        {
            state = in_buffer >> 24;
            in_buffer = 0;
            byte_in_id = 4;
        }
        if (byte_in_id == 0)
        {
            in_buffer = 0;
            byte_in_id = 4;
        }
        break;
    case PREPARE_TASK_TRANSFER:
        buffer[bufferIndex++] = read_data_register();
        if (bufferIndex == 8)
        {
            tInfo = (TaskInformation_t*) allocate(sizeof(TaskInformation_t));
            if (!tInfo)
                invoke_panic(OUT_OF_MEMORY);
            swap(buffer);
            tInfo->task_size = 512; 
            // tInfo->task_size = *((unsigned int*) buffer); 
            tInfo->start_adress = (char*) allocate(tInfo->task_size); 
            
            if (!tInfo->start_adress)
                invoke_panic(OUT_OF_MEMORY);
            state = TRANSFER_TASK_BYTES;
            bufferIndex = 0;

            for (unsigned int i = 0; i < 16; i++)
                buffer[i] = 0;

            p = tInfo->start_adress;
        }
        break;
    case TRANSFER_TASK_BYTES:
        os_memcpy(p++, read_data_register());
        // setup_transfer(&"foobar", 6);
        // setup_transfer("foobar!\n\r", 9);
        if (++bufferIndex == tInfo->task_size)
        {
            deallocate((unsigned int*) tInfo);
            create_task((void (*)()) tInfo->start_adress, (unsigned int) tInfo->start_adress);
            state = RX_READY;
            bufferIndex = 0;
        }
        break;
    default:
        break;
    }    
}
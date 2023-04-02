#include "uart.h"
#include "uart_common.h"
#include "data/list.h"
#include "memory.h"
#include "exception.h"
#include "process/scheduler.h"
#include "panic.h"
#include "dma.h"

UartStates_t state;
TaskInformation_t tInfo;
List_t *transfer_list = NULL;

unsigned int in_buffer;
unsigned int byte_in_id;
unsigned int bufferIndex;

char buffer[BUFFERSIZE];
char *p;

void init_transfer_handler(void)
{
    transfer_list = (List_t*) new_list();
}

void __attribute__((optimize("O0"))) setup_transfer(char* address, unsigned int length, TransferType_t type)
{
    if (transfer_list->size > MAX_WAITING_TRANSFERS)
        return;

    if (transfer_list->size == 0)
        wakeup_pid(1);

    TransferInfo_t* t = (TransferInfo_t*) allocate(sizeof(TransferInfo_t));
    
    if (!t)
        invoke_panic(OUT_OF_MEMORY);
    
    t->start_adress = address;
    t->length = length;
    t->type = type;
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
            unsigned int remaining = TX_LENGTH;
            char *m;
            TransferInfo_t* transfer = (TransferInfo_t*) node->data;
            switch (transfer->type)
            {
            case GENERIC:
                break;
            case MEM_ADDRESS:
                m = "AAAABBBB";
                print(m, 8);
                remaining -= 8;
                break;
            case STATISTIC:
                m = "BBBBAAAA";
                print(m, 8);
                remaining -= 8;
                break;
            case RPM:
                m = "CCCCAAAA";
                print(m, 8);
                remaining -= 8;
                break;
            case PLANEPOSITION:
                m = "DDDDAAAA";
                print(m, 8);
                remaining -= 8;
                break;
            default:
                break;
            }
            print(transfer->start_adress, transfer->length);
            remaining -= transfer->length;

            char zero = '1';
            while (remaining-- != 0)
                print(&zero, 1);

            if (deallocate((unsigned int*) transfer) == 0)
                invoke_panic(MEMORY_DEALLOC_FAILED);
            if (deallocate((unsigned int*) node) == 0)
                invoke_panic(MEMORY_DEALLOC_FAILED);
        }
        else
        {
            block_current_task();
            // SV_YIELD_TASK;
        }
    }    
}

void init_isr(void)
{
    byte_in_id = 4;
    state = RX_READY;
    bufferIndex = 0;
    in_buffer = 0;
    for (unsigned int i = 0; i < BUFFERSIZE; i++)
        buffer[i] = 0;
}

static char bytes_received = 0;
char uart_rx_buffer[4];
void __attribute__((interrupt))  __attribute__((optimize("O0"))) uart_isr_handler(void)
{
    uart_rx_buffer[bytes_received++] = read_data_register();
    if (bytes_received != 4)
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
        tInfo.start_adress = (char*) allocate(tInfo.task_size); 
        
        if (!tInfo.start_adress)
            invoke_panic(OUT_OF_MEMORY);

        setup_transfer(&tInfo.start_adress, 4, MEM_ADDRESS);

        state = RX_READY;
        break;
    case TRANSFER_TASK_BYTES:
        // set up dma RX
        // os_memcpy(tInfo.start_adress++, rx_byte);
        if (++bufferIndex == tInfo.task_size)
        {
            create_task((void (*)()) tInfo.start_adress, (unsigned int) tInfo.start_adress);
            state = RX_READY;
        }
        break;
    case REQUEST_STATISTIC:
        wakeup_pid(2);
        state = RX_READY;
        return;
    case REBOOT:
        state = RX_READY;
        soft_reset();
        return;
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
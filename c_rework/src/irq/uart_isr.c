#include "devices/uart.h"


UartStates_t state;
TaskInformation_t *tInfo = NULL;
unsigned int in_buffer = 0;
unsigned int byte_in_id;
unsigned int bufferIndex;
char buffer[16];
char *p;

void init_isr(void)
{
    byte_in_id = 4;
    state = RX_READY;
    bufferIndex = 0;
}

void __attribute__((interrupt)) __attribute__ ((always_inline)) uart_isr_handler(void)
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
            swap(buffer);
            tInfo->task_size = *((unsigned int*) buffer); 
            tInfo->start_adress = (char*) allocate(tInfo->task_size); 
            state = TRANSFER_TASK_BYTES;
            bufferIndex = 0;
            p = tInfo->start_adress;
        }
        break;
    case TRANSFER_TASK_BYTES:
        os_memcpy(p++, read_data_register());
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
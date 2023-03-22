#include "devices/uart.h"
#include "data/list.h"
#include "memory.h"
#include "exception.h"
#include "process/scheduler.h"
#include "panic.h"

UartStates_t state;
TaskInformation_t *tInfo = NULL;
TransferInfo_t transfer_list[MAX_WAITING_TRANSFERS];

unsigned int in_buffer;
unsigned int byte_in_id;
unsigned int bufferIndex;
unsigned int num_of_waiting_transfers;
char buffer[BUFFERSIZE];
char *p;

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
    SingleLinkedNode_t* node;
    init_transfer_handler();
    unsigned int j = 0;
    while (1)
    {
        if (num_of_waiting_transfers)
        {
            print(transfer_list[j].start_adress, transfer_list[j].length);
            num_of_waiting_transfers--;
            j++;
        }
        else
        {
            j = 0;
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
    bufferIndex = 0;
    in_buffer = 0;
    for (unsigned int i = 0; i < BUFFERSIZE; i++)
        buffer[i] = 0;
}

void __attribute__((interrupt)) uart_isr_handler(void)
{
    switch (state)
    {
    case RX_READY:
        unsigned int content = read_data_register();
        in_buffer |= content << ((--byte_in_id) * 8);
        if ((in_buffer & 0xFFFFFF) == MAGIC)
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
        if (bufferIndex == 4)
        {
            tInfo = (TaskInformation_t*) allocate(sizeof(TaskInformation_t));
            if (!tInfo)
                invoke_panic(OUT_OF_MEMORY);
            
            swap(buffer);
            tInfo->task_size = (unsigned int) *((unsigned int*) buffer); 
            tInfo->start_adress = (char*) allocate(tInfo->task_size); 
            
            if (!tInfo->start_adress)
                invoke_panic(OUT_OF_MEMORY);

            // done because of pyserial testing to receive mem adress everytime!
            // state = RX_READY;
            
            state = TRANSFER_TASK_BYTES;

            bufferIndex = 0;
            setup_transfer((char*) &tInfo->start_adress, 4);

            for (unsigned int i = 0; i < BUFFERSIZE; i++)
                buffer[i] = 0;

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
    case REQUEST_STATISTIC:
        wakeup_pid(2);
        state = RX_READY;
        unsigned int dummy = read_data_register();
        return;
        // break;
    case ALTER_SPEED:
        buffer[bufferIndex++] = read_data_register();
        if (bufferIndex == 4)
        {
            volatile unsigned int pwm_speed = (buffer[1] - 0x30) * 100 + (buffer[2] - 0x30) * 10 + (buffer[3] - 0x30);
            volatile unsigned int engine_no = (buffer[0] - 0x30);
            bufferIndex = 0;
            state = RX_READY;

            // should be moved into own function soon
            switch (engine_no)
            {
            case 0:
                // ccr1
                WRITE_REGISTER(0x40000434, pwm_speed);
                break;
            case 1:
                WRITE_REGISTER(0x40000438, pwm_speed);
                break;
            case 2:
                WRITE_REGISTER(0x4000043C, pwm_speed);
                break;
            case 3:
                WRITE_REGISTER(0x40000440, pwm_speed);
                break;            
            default:
                break;
            }
        }
        break; 
    case REQUEST_RPM:
        wakeup_pid(-1);
        state = RX_READY;
        unsigned int dummy1 = read_data_register();
        return;
    case REBOOT:
        state = RX_READY;
        unsigned int dummy2 = read_data_register();
        soft_reset();
        return;
    case REQUEST_TEST_RESULT:
        unsigned int dummy3 = read_data_register();
        char *test = "DDDDEEEE";
        print(test, 8);
        print((char*) 0x20000000, 4);
        WRITE_REGISTER(0x20000000, 0);
        state = RX_READY;
        return;
    case REQUEST_POSITION:
        unsigned int dummy4 = read_data_register();
        wakeup_pid(3);
        state = RX_READY;
        return;
    default:
        break;
    }
}
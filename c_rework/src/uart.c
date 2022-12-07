#include "uart.h"
#include "lang.h"
#include "memory.h"
#include "process/task.h"


char buffer[16];
unsigned int in_buffer = 0;
volatile unsigned int byte_in_id;
unsigned int index = 0;

unsigned int flush = 0xAA; 

char *p;

UartStates_t state;
TaskInformation_t *tInfo = NULL;

const unsigned int magic = 0x123456;
// void send_nil();
void init_uart()
{
    *((unsigned int*) USART1_CR) = *((unsigned int*) USART1_CR) | 0x1 << UE | 0x1 << RXNEIE | 0x1 << RE | 0x1 << TE;
    byte_in_id = 4;
    state = RX_READY;
    index = 0;

}

// void print_char(char c)
// {
//     WRITE_REGISTER(USART1_DR,(c));
//     while (!((READ_REGISTER(USART1_SR) & 0x80) != 0));
// }

// void print(char *str)
// {
//     while (*str)
//     {
//         WRITE_REGISTER(USART1_DR,(*str));
//         while (!((READ_REGISTER(USART1_SR) & 0x80) != 0));
//         str++;
//     }
// }

// void send_nil()
// {
//     char *nil = '\0';
//     WRITE_REGISTER(USART1_DR,(nil));
//     while (!((READ_REGISTER(USART1_SR) & 0x80) != 0));
// }

// void send_number(uint32_t number)
// {
//     uint32_t converted[20];
//     uint32_t n = number;
//     int cnt = 0;

//     if (n == 0)
//         print_char(0 + 0x30);
//     else
//     {
//         while (n > 0)
//         {
//             converted[cnt] = (n % 10) + 0x30;
//             n /= 10;
//             cnt++;
//         }
//         for (int i = cnt - 1; i >= 0; i--)
//         {
//             print_char(converted[i]);
//         }
//     }
//     print_char('\n');
// }

// uint32_t pow_base_16(uint32_t exp)
// {
//     uint32_t base = 16;
//     if (exp == 0)
//     {
//         return 1;
//     }
//     else if (exp == 1)
//     {
//         return 16;
//     }
//     else
//     {
//         for (uint32_t n = 0; n < exp - 1; n++)
//         {
//             base *= 16;
//         }
//     }
//     return base;
// }

// uint32_t hex2int(char ch)
// {
//     if (ch >= '0' && ch <= '9')
//         return ch - '0';
//     if (ch >= 'A' && ch <= 'F')
//         return ch - 'A' + 10;
//     if (ch >= 'a' && ch <= 'f')
//         return ch - 'a' + 10;
//     return -1;
// }


void __attribute__((optimize("O0"))) uart_isr_handler(void)
{
        //    } else */ if rx_data as char == 'f' {
        //     task_target = 0x20001000;
        //     // flush memory
        //     for i in (0x0..0x60).step_by(0x4) {
        //         volatile_store((task_target + i) as *mut u32, 0x0);
        //     }
        //     task_target = 0x20001000;
        //     task_byte = 0;
        //     task_word = 0;
        //     return;
        //             if rcv_state == RCV_STATE::TASK {
        //     // task_buffer[task_byte as usize] = rx_data;
        //     // task_byte += 1;
        //     // // 2 3 0 1
        //     task_word |= (rx_data as u32) << task_byte * 8;
        //     if task_byte == 3 {
        //         volatile_store(task_target as *mut u32, task_word);
        //         task_target += 0x4;
        //         task_byte = 0;
        //         task_word = 0;
        //     } else {
        //         task_byte += 1;
        //     }

    switch (state)
    {
    case RX_READY:
        in_buffer |= (*((unsigned int*) USART1_DR)) << ((--byte_in_id) * 8);
        if ((in_buffer & 0xFFFFFF) == magic)
        {
            state = in_buffer >> 24;
            in_buffer = 0;
            byte_in_id = 4;
        }
        break;
    case PREPARE_TASK_TRANSFER:
        buffer[index++] = (*((unsigned int*) USART1_DR));
        if (index == 8)
        {
            tInfo = (TaskInformation_t*) allocate(sizeof(TaskInformation_t));
            swap(buffer);
            tInfo->task_size = *((unsigned int*) buffer); 
            tInfo->start_adress = (char*) allocate(tInfo->task_size); 
            state = TRANSFER_TASK_BYTES;
            index = 0;
            p = tInfo->start_adress;
        }
        break;
    case TRANSFER_TASK_BYTES:
        if (index++ <= tInfo->task_size)
            os_memcpy(p++, *((char*) USART1_DR));

        if (index == tInfo->task_size)
        {
            deallocate(tInfo);
            create_task((void (*)()) tInfo->start_adress);
            state = RX_READY;
        }
        break;
    default:
        break;
    }
}


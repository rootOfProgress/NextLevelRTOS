#include "memory.h"
#include "lang.h"
#include "test.h"
#include "process/scheduler.h"
#include "devices/uart.h"

static unsigned int* MEM_TABLE_START = 0;
static unsigned int* USEABLE_MEM_START = 0;
const unsigned int NUM_OF_SLOTS = 60;
unsigned int mutex;

MemoryStatistic_t mstat;

void swap(char* buffer)
{
    // char* p = (char*) buffer;
    char tmp;

    // swap 1 - 4
    tmp = *buffer;
    *buffer = *(buffer+3);
    *(buffer+3) = tmp;

    // swap 2 - 3
    tmp = *(buffer+1);
    *(buffer+1) = *(buffer+2);
    *(buffer+2) = tmp;
}


void swap_endianess(char* buffer)
{
    // char* p = (char*) buffer;
    char tmp;

    // swap 1 - 4
    tmp = *buffer;
    *buffer = *(buffer+1);
    *(buffer+1) = tmp;

    // swap 2 - 3
    tmp = *(buffer+2);
    *(buffer+2) = *(buffer+3);
    *(buffer+3) = tmp;
}

void memset_byte(void* dest, unsigned int number_of_bytes, char pattern)
{
    char* dest_1 = (char*) dest;
    for (unsigned int i = 0; i < number_of_bytes; i++)
    {
        *dest_1 = pattern;
        dest_1++;
    }
}


void init_allocator(unsigned int start_os_section, unsigned int* ram_size) {
    while ((start_os_section & 0x3) != 0) {
        start_os_section += 1;
    }
    MEM_TABLE_START = (unsigned int*) start_os_section;
    USEABLE_MEM_START = MEM_TABLE_START + NUM_OF_SLOTS;

    for (unsigned int index = 0; index < NUM_OF_SLOTS; index += 1)
    {
        *(MEM_TABLE_START + index) = 0x0000FFFE;
    }
    mutex = 0;
    memset_byte((void*) &mstat, sizeof(MemoryStatistic_t), 0); 
    mstat.ram_size = (unsigned int) ram_size;
}

unsigned int __attribute__((optimize("O0"))) deallocate(unsigned int* address) {
    // return;
    lock_mutex((void*) mutex);

    unsigned int address_offset = (unsigned int) address - (unsigned int) USEABLE_MEM_START;
    for (unsigned int index = 0; index < NUM_OF_SLOTS; index++)
    {
        unsigned int alloc_entry = *(MEM_TABLE_START + index);
        if ((alloc_entry >> 16) == address_offset) {
            mstat.num_of_deallocs++;
            *(MEM_TABLE_START + index) = alloc_entry & ~1;
            unlock_mutex((void*) mutex);

            return 1;
        }
    }
    unlock_mutex((void*) mutex);

    return 0;
}

void __attribute__ ((cold)) update_memory_statistic(void)
{
    // mstat.total_byte_alloced = 0;

    unsigned int msp_position;
    __asm volatile ("mrs %0, msp" : "=r"(msp_position));

    mstat.total_byte_alloced = (RAM_START + mstat.ram_size) - msp_position;
    mstat.total_byte_alloced += (unsigned int) MEM_TABLE_START - RAM_START;
    mstat.total_byte_alloced += NUM_OF_SLOTS * sizeof(unsigned int);

    // count alloced spaces
    for (unsigned int index = 0; index < NUM_OF_SLOTS; index += 1)
    {
        unsigned int entry = *(MEM_TABLE_START + index);
        if ((entry & 1) == 1)
            mstat.total_byte_alloced += (entry & 0xFFFE) >> 1;
    }    
    Node_t* q = task_queue->head;
    do
    {
            Tcb_t* t = q->data;
            mstat.total_byte_used = t->memory_upper_bound - t->sp;
    } while (q != task_queue->head);
}

unsigned int* allocate(unsigned int size) {
    lock_mutex((void*) mutex);
    unsigned int requested_size = size;
    unsigned int next_useable_chunk = 0;

    while ((requested_size & 0x3) != 0) {
        requested_size += 1;
    }

    /*
     *  CHUNK LIST LAYOUT
     *
     *  start_of_memory_block: 0x2000_0100 + OFFSET
     *  | OFFSET , SIZE, IS_OCUPIED | [31..16, 15..1, 0]
     *
     *
     **/
    for (unsigned int index = 0; index < NUM_OF_SLOTS; index++)
    {
        // 47 possible allocs @todo WRONG COUNT!!
        unsigned int memory_entry = *(MEM_TABLE_START + index);

        // check if occupied
        if ((memory_entry & 1) == 1) {
            // get size and add to offset
            next_useable_chunk += (memory_entry & 0xFFFE) >> 1;
            continue;
        }

        // if ((memory_entry & 1) == 0) {
            // check if chunk was already in use: Offset is not 0 
        if ((((memory_entry >> 16) != 0) && ((memory_entry & 0xFFFE) >> 1) == requested_size))
        {
            unsigned int old_size = (memory_entry & 0xFFFE) >> 1;
            memory_entry &= ~0xFFFF;
            unsigned int remaining = old_size - requested_size;

            // untested!
            if (remaining > 0)
            {
                unsigned int j = index;
                while (*(MEM_TABLE_START + j++) != 0x0000FFFE) {};
                *(MEM_TABLE_START + j) = ((memory_entry >> 16) + requested_size) | remaining << 1 | 0; 
                // mstat.num_of_fractial_allocs++;
            }

            *(MEM_TABLE_START + index) = memory_entry | (requested_size << 1) | 0x1;
            mstat.num_of_allocs++;
            // SV_STE;
            unlock_mutex((void*) mutex);
            return (unsigned int*) ((memory_entry >> 16) + (unsigned int) USEABLE_MEM_START);
        } 
        // check if size fits on new chunk
        if (((memory_entry & 0xFFFE) >> 1) >= requested_size)
        {
            // update offsetadress, size, mark as occupied
            memory_entry = (next_useable_chunk << 16) | (requested_size << 1) | 1;

            // write back changes
            *(MEM_TABLE_START + index) = memory_entry;
            mstat.num_of_allocs++;
            // SV_STE;
            unlock_mutex((void*) mutex);

            return (unsigned int*) ((memory_entry >> 16) + (unsigned int) USEABLE_MEM_START);
        }
        next_useable_chunk += (memory_entry & 0xFFFE) >> 1;
       // }
    }
    unlock_mutex((void*) mutex);

    // SV_STE;
    return NULL;
}
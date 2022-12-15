#include "memory.h"
#include "lang.h"
#include "test.h"
#include "process/scheduler.h"

static unsigned int* MEM_TABLE_START = 0;
static unsigned int* USEABLE_MEM_START = 0;
const unsigned int NUM_OF_SLOTS = 60;

MemoryStatistic_t *mstat = NULL;

#ifdef SELF_CHECK

void __attribute__((optimize("O0"))) do_selfcheck_memory()
{
    unsigned int test_a = 1;
    unsigned int test_b = 1;
    unsigned int test_c = 1;
    unsigned int test_d = 1;
    unsigned int test_e = 1;
    unsigned int test_f = 1;
    unsigned int test_g = 1;
    unsigned int test_h = 1;
    unsigned int test_i = 1;
    unsigned int test_j = 1;
    unsigned int test_k = 1;
    unsigned int test_l = 1;
    unsigned int test_m = 1;
    unsigned int test_n = 1;

    // test_a: check init
    for (int index = 0; index < 30; index += 1)
    {
        if (*(MEM_TABLE_START + index) != 0x0000FFFE)
            test_a = 0;
    }

    // test_b: check alloc 4
    unsigned int mem_1 = (unsigned int) allocate(4);
    if (mem_1 != (unsigned int) USEABLE_MEM_START)
        test_b = 0;

    // test_c: check alloc 8
    unsigned int mem_2 = (unsigned int) allocate(8);
    if (mem_2 != mem_1 + 4)
        test_c = 0;

    // test_d: check alloc 9
    unsigned int mem_3 = (unsigned int) allocate(9);
    if (mem_3 != mem_1 + 12)
        test_d = 0;

    // test_e: check alloc 9
    unsigned int mem_4 = (unsigned int) allocate(999999);
    if (mem_4 != 0)
        test_e = 0;

    // test_e: check alloc 9
    unsigned int mem_5 = (unsigned int) allocate(2);
    if (mem_5 != mem_1 + 24)
        test_f = 0;

    // deallocate(mem_1);
    deallocate(mem_2);
    deallocate(mem_3);
    deallocate(mem_4);
    deallocate(mem_5);
    unsigned int mem_5_new = (unsigned int) allocate(2);
    if (mem_5 != mem_5_new)
        test_g = 0;
    unsigned int mem_4_new = (unsigned int) allocate(999999);
    if (mem_4 != mem_4_new)
        test_h = 0;

    

    // restore all
    for (int index = 0; index < 30; index += 1)
    {
        *(MEM_TABLE_START + index) = 0x0000FFFE;
    }


}
#endif

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

void init_allocator(unsigned int start_os_section) {
    while (start_os_section % 4 != 0) {
        start_os_section += 1;
    }
    MEM_TABLE_START = (unsigned int*) start_os_section;
    USEABLE_MEM_START= MEM_TABLE_START + NUM_OF_SLOTS;

    // todo
    for (unsigned int index = 0; index < NUM_OF_SLOTS; index += 1)
    {
        // unsigned int foo = *(MEM_TABLE_START + index);
        *(MEM_TABLE_START + index) = 0x0000FFFE;
    }

    mstat = (MemoryStatistic_t*) allocate(sizeof(MemoryStatistic_t));
    mstat->num_of_allocs = 0;
    mstat->num_of_deallocs = 0;
    mstat->num_of_fractial_allocs = 0;
    mstat->os_data_end = (unsigned int) USEABLE_MEM_START;
    #ifdef SELF_CHECK
        do_selfcheck_memory();
    #endif
}

void  deallocate(unsigned int* address) {
    // return;
    unsigned int address_offset = (unsigned int) address - (unsigned int) USEABLE_MEM_START;
    for (unsigned int index = 0; index < NUM_OF_SLOTS; index++)
    {
        unsigned int alloc_entry = *(MEM_TABLE_START + index);
        if ((alloc_entry >> 16) == address_offset) {
            mstat->num_of_deallocs++;
            *(MEM_TABLE_START + index) = alloc_entry & ~1;
            return;
        }
    }
}

void update_statistic(void)
{
    mstat->total_byte_alloced = 0;
    // count alloced spaces
    for (unsigned int index = 0; index < NUM_OF_SLOTS; index += 1)
    {
        unsigned int entry = *(MEM_TABLE_START + index);
        if ((entry & 1) == 1)
            mstat->total_byte_alloced += (entry & 0xFFFE) >> 1;
    }    
    Node_t* q = task_queue->head;
    do
    {
            Tcb_t* t = q->data;
            mstat->total_byte_used = t->memory_upper_bound - t->sp;
    } while (q != task_queue->head);
    
}

unsigned int* allocate(unsigned int size) {
    unsigned int requested_size = size;
    unsigned int next_useable_chunk = 0;

    while ((requested_size % 4) != 0) {
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
                mstat->num_of_fractial_allocs++;
            }

            *(MEM_TABLE_START + index) = memory_entry | (requested_size << 1) | 0x1;
            mstat->num_of_allocs++;
            return (unsigned int*) ((memory_entry >> 16) + (unsigned int) USEABLE_MEM_START);
        } 
        // check if size fits on new chunk
        if (((memory_entry & 0xFFFE) >> 1) >= requested_size)
        {
            // update offsetadress, size, mark as occupied
            memory_entry = (next_useable_chunk << 16) | (requested_size << 1) | 1;

            // write back changes
            *(MEM_TABLE_START + index) = memory_entry;
            mstat->num_of_allocs++;
            return (unsigned int*) ((memory_entry >> 16) + (unsigned int) USEABLE_MEM_START);
        }
        next_useable_chunk += (memory_entry & 0xFFFE) >> 1;
       // }
    }
    return NULL;
}
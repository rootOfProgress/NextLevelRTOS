#include "memory.h"
#include "lang.h"
#include "test.h"
static unsigned int* MEM_TABLE_START = 0;
static unsigned int* USEABLE_MEM_START = 0;
static unsigned int allocs = 0;
static unsigned int deallocs = 0;
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
    USEABLE_MEM_START= MEM_TABLE_START + 80;

    // todo
    for (int index = 0; index < 80; index += 1)
    {
        // unsigned int foo = *(MEM_TABLE_START + index);
        *(MEM_TABLE_START + index) = 0x0000FFFE;
    }
    #ifdef SELF_CHECK
        do_selfcheck_memory();
    #endif
}

void  deallocate(void* address) {
    // return;
    unsigned int address_offset = (unsigned int) address - (unsigned int) USEABLE_MEM_START;
    for (unsigned int index = 0; index < 80; index++)
    {
        unsigned int alloc_entry = *(MEM_TABLE_START + index);
        if ((alloc_entry >> 16) == address_offset) {
            deallocs++;
            *(MEM_TABLE_START + index) = alloc_entry & ~1;
            return;
        }
    }
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
    for (unsigned int index = 0; index < 80; index++)
    {
        // 47 possible allocs @todo WRONG COUNT!!
        unsigned int memory_entry = *(MEM_TABLE_START + index);

        // check if occupied
        if ((memory_entry & 1) == 1) {
            // get size and add to offset
            next_useable_chunk += (memory_entry & 0xFFFE) >> 1;
            continue;
        }

        if ((memory_entry & 1) == 0) {
            // check if chunk was already in use: Offset is not 0 
            if ((((memory_entry >> 16) != 0) && ((memory_entry & 0xFFFE) >> 1) == requested_size))
            {
                *(MEM_TABLE_START + index) = memory_entry | 1;
                allocs++;
                return (unsigned int*) ((memory_entry >> 16) + (unsigned int) USEABLE_MEM_START);
            } 
            // check if size fits on new chunk
            if (((memory_entry & 0xFFFE) >> 1) >= requested_size)
            {
                // update offsetadress, size, mark as occupied
                memory_entry = (next_useable_chunk << 16) | (requested_size << 1) | 1;

                // write back changes
                *(MEM_TABLE_START + index) = memory_entry;
                allocs++;
                return (unsigned int*) ((memory_entry >> 16) + (unsigned int) USEABLE_MEM_START);
            }
            next_useable_chunk += (memory_entry & 0xFFFE) >> 1;
        }
    }
    return NULL;
}
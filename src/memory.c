#include "../include/memory.h"

static int* MEM_TABLE_START = 0;
static int* USEABLE_MEM_START = 0;


//     static mut _sbss: u8;
//     static mut _ebss: u8;

//     static mut _sdata: u8;
//     static mut _edata: u8;
//     static _sidata: u8;
// }

// extern "C" {
//     fn write_memory(value: u32, adress: u32);
//     fn read_memory(adress: u32) -> u32;
// }

void init(int start_os_section) {
    while (start_os_section % FOURBYTE != 0) {
        start_os_section += 1;
    }
    *MEM_TABLE_START = start_os_section;
    *USEABLE_MEM_START= *MEM_TABLE_START + 0x30;
    for (int index = 0x00; index < 0x02F; index += FOURBYTE)
    {
        *(MEM_TABLE_START + index) = 0x0000FFFE;
    }
}

void memory_mng_deallocate(int address) {
    int address_offset = address - *USEABLE_MEM_START;
    for (int index = 0x00; index < 0x02F; index += FOURBYTE)
    {
        int alloc_entry = *(MEM_TABLE_START + index);
        if ((alloc_entry >> 16) == address_offset) {
            *(MEM_TABLE_START + index) = alloc_entry & !1;
            return;
        }
    }
}

// #[inline(always)]
MemoryResult_t allocate(int size) {
    unsigned int requested_size = size;
    unsigned int next_useable_chunk = 0;

    while ((requested_size % FOURBYTE) != 0) {
        requested_size += 1;
    }

    /*
     *  CHUNK LIST LAYOUT
     *
     *  start_of_memory_block: 0x2000_0100 + OFFSET
     *  adress  0x00 | OFFSET , SIZE, IS_OCUPIED | [31..16, 15..1, 0]
     *
     *
     **/
    for (int index = 0x00; index < 0x02F; index += FOURBYTE)
    {
        // 47 possible allocs @todo WRONG COUNT!!
        unsigned int meta_of_data_chunk = *(MEM_TABLE_START + index);

        // check if occupied
        if ((meta_of_data_chunk & 1) == 1) {
            // get size and add to offset
            next_useable_chunk += (meta_of_data_chunk >> 1) & !(0xFFFF0000);
            continue;
        }

        // check if size fits
        if (((meta_of_data_chunk >> 1) & !(0xFFFF0000)) >= requested_size) {
            // update offsetadress, size, mark as occupied
            meta_of_data_chunk = (next_useable_chunk << 16) | (requested_size << 1) | 1;

            // write back changes
            *(MEM_TABLE_START + index) = meta_of_data_chunk;

            int start_address = (meta_of_data_chunk >> 16) + *USEABLE_MEM_START;

            MemoryResult_t result = {
                .start_address = start_address,
                .end_address = start_address + size,
            };
            return result;
        }
    }
    MemoryResult_t result = {
        .start_address = 0,
        .end_address = 0,
    };
    return result;
}
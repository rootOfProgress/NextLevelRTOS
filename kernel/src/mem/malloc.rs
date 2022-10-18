static mut MEM_TABLE_START: *const u32 = core::ptr::null();
static mut USEABLE_MEM_START: *const u32 = core::ptr::null();
const WORD: u32 = 0x4;

use core::intrinsics::{volatile_load, volatile_store};
use core::ptr::swap;

extern "C" {
    static mut _sbss: u8;
    static mut _ebss: u8;

    static mut _sdata: u8;
    static mut _edata: u8;
    static _sidata: u8;
}

extern "C" {
    fn write_memory(value: u32, adress: u32);
    fn read_memory(adress: u32) -> u32;
}

pub unsafe fn init(start_os_section: usize, bar: usize) {
    // @bug
    let mut start = start_os_section;
    while (start % 4 != 0) {
        start += 1;
    }
    MEM_TABLE_START = start as *const u32;
    USEABLE_MEM_START = (MEM_TABLE_START as u32 + 0x30) as *const u32;
    for index in (0x00..0x2F).step_by(0x04) {
        volatile_store((MEM_TABLE_START as u32 + index) as *mut u32, 0x0000_FFFE);
    }
}

pub unsafe fn memory_mng_deallocate(address: u32) {
    let address_offset: u32 = address - USEABLE_MEM_START as u32 ;
    for index in (0x00..0x2F).step_by(0x04) {
        let alloc_entry: u32 = volatile_load((MEM_TABLE_START as u32 + index) as *mut u32);
        if (alloc_entry >> 16) == address_offset {
            volatile_store((MEM_TABLE_START as u32 + index) as *mut u32, alloc_entry & !1);
            return;
        }
    }
}

pub fn memory_mng_allocate(size: u32) -> u32 {
    unsafe {
        allocate(size)
    }
}

pub fn memory_mng_allocate_process(size: u32) -> u32 {
    unsafe {
        allocate(size) + size
    }
}

#[inline(always)]
unsafe fn allocate(size: u32) -> u32 {
    let mut requested_size = size;
    let mut next_useable_chunk = 0;

    // replace soon
    while (requested_size % 4) != 0 {
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
    for index in (0x00..0x2F).step_by(0x04) { // 47 possible allocs
        let mut meta_of_data_chunk = volatile_load((MEM_TABLE_START as u32 + index) as *const u32);

        // check if occupied
        if (meta_of_data_chunk & 1) == 1 {
            // get size and add to offset
            next_useable_chunk += (meta_of_data_chunk >> 1) & !(0xFFFF_0000);
            continue;
        }

        // check if size fits
        if ((meta_of_data_chunk >> 1) & !(0xFFFF_0000)) >= requested_size {
            // clear old meta info 
            meta_of_data_chunk &= !(0xFFFFFFFF);

            // update offsetadress, size, mark as occupied
            meta_of_data_chunk = (next_useable_chunk << 16) | (requested_size << 1) | 1;

            // write back changes
            volatile_store((MEM_TABLE_START as u32 + index) as *mut u32, meta_of_data_chunk);

            return (meta_of_data_chunk >> 16) + USEABLE_MEM_START as u32 ;
        }
    }
    0
}

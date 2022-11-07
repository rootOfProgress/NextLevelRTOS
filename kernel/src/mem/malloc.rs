use core::cell::Cell;
use core::intrinsics::{volatile_load, volatile_store};

static mut MEM_TABLE_START: Cell<u32> = Cell::new(0);
static mut USEABLE_MEM_START: Cell<u32> = Cell::new(0);

pub struct MemoryResult {
    pub start_address: u32,
    pub end_address: u32,
}

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

pub unsafe fn init(mut start_os_section: Cell<u32>) {
    while start_os_section.get() % 4 != 0 {
        *start_os_section.get_mut() += 1;
    }
    *MEM_TABLE_START.get_mut() = start_os_section.get();
    *USEABLE_MEM_START.get_mut() = MEM_TABLE_START.get() + 0x30;
    for index in (0x00..0x2F).step_by(0x04) {
        volatile_store((MEM_TABLE_START.get() + index) as *mut u32, 0x0000_FFFE);
    }
}

pub unsafe fn memory_mng_deallocate(address: u32) {
    let address_offset: u32 = address - USEABLE_MEM_START.get() as u32;
    for index in (0x00..0x2F).step_by(0x04) {
        let alloc_entry: u32 = volatile_load((MEM_TABLE_START.get() + index) as *mut u32);
        if (alloc_entry >> 16) == address_offset {
            volatile_store(
                (MEM_TABLE_START.get() + index) as *mut u32,
                alloc_entry & !1,
            );
            return;
        }
    }
}

// pub fn memory_mng_allocate(size: u32) -> u32 {
//     unsafe {
//         allocate(size)
//     }
// }

// pub fn memory_mng_allocate_process(size: u32) -> u32 {
//     unsafe {
//         // @todo
//         // configure MPU for proc id
//         // return start & end size
//         allocate(size) + size /* - 0x04 */
//     }
// }

// #[inline(always)]
pub unsafe fn allocate(size: u32) -> Option<MemoryResult> {
    let mut requested_size = size;
    let mut next_useable_chunk = 0;

    // panic!("asd");
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
    for index in (0x00..0x2F).step_by(0x04) {
        // 47 possible allocs @todo WRONG COUNT!!
        let mut meta_of_data_chunk = volatile_load((MEM_TABLE_START.get() + index) as *const u32);

        // check if occupied
        if (meta_of_data_chunk & 1) == 1 {
            // get size and add to offset
            next_useable_chunk += (meta_of_data_chunk >> 1) & !(0xFFFF_0000);
            continue;
        }

        // check if size fits
        if ((meta_of_data_chunk >> 1) & !(0xFFFF_0000)) >= requested_size {
            // update offsetadress, size, mark as occupied
            meta_of_data_chunk = (next_useable_chunk << 16) | (requested_size << 1) | 1;

            // write back changes
            volatile_store(
                (MEM_TABLE_START.get() + index) as *mut u32,
                meta_of_data_chunk,
            );

            let start_address = (meta_of_data_chunk >> 16) + USEABLE_MEM_START.get();
            return Some(MemoryResult {
                start_address: start_address,
                end_address: start_address + size,
            });
        }
    }
    None
}

#[no_mangle]
pub extern "C" fn MemoryManagementFault() {
    loop {}
}

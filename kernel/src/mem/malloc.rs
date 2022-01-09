const JOURNAL_BASE: u32 = 0x2000_0000;
const JOURNAL_NUM_OF_ELEMENTS: u32 = JOURNAL_BASE;
const JOURNAL_START: u32 = JOURNAL_BASE + 0x04;

const MEM_BLOCK_START: u32 = 0x2000_0048;
const ADDR_OF_HIGHEST_FREE_BLOCK: u32 = MEM_BLOCK_START + 0x04;

pub unsafe fn init() {
    core::ptr::write_volatile(MEM_BLOCK_START as *mut u32, ADDR_OF_HIGHEST_FREE_BLOCK);
    core::ptr::write_volatile(JOURNAL_NUM_OF_ELEMENTS as *mut u32, 0x0000_0000);
}

pub unsafe fn get_mem(requested_size: u32) -> u32 {
    if core::ptr::read_volatile(JOURNAL_NUM_OF_ELEMENTS as *const u32) != 0 {
        // next fit
        let entries = core::ptr::read_volatile(JOURNAL_NUM_OF_ELEMENTS as *const u32);
        for journal_entry in 0..entries {
            let journal_entry_addr = JOURNAL_START + journal_entry * 0x04;
            let free_entry = core::ptr::read_volatile(journal_entry_addr as *const u32);
            let size_available = core::ptr::read_volatile(free_entry as *const u32);
            if requested_size <= size_available {
                core::ptr::write_volatile(journal_entry_addr as *mut u32, 0x0000_0000);

                let journal_size = core::ptr::read_volatile(JOURNAL_NUM_OF_ELEMENTS as *const u32);
                let end_adress = JOURNAL_BASE + journal_size * 0x04;

                if journal_size == 1 {
                    core::ptr::write_volatile(journal_entry_addr as *mut u32, 0x0000_0000);
                } else {
                    core::ptr::swap(end_adress as *mut u32, journal_entry_addr as *mut u32);
                }
                core::ptr::write_volatile(
                    JOURNAL_NUM_OF_ELEMENTS as *mut u32,
                    core::ptr::read_volatile(JOURNAL_NUM_OF_ELEMENTS as *const u32) - 1,
                );
                core::ptr::write_volatile(free_entry as *mut u32, requested_size);
                return free_entry + 0x4;
            }
        }
        let ahf = core::ptr::read_volatile(MEM_BLOCK_START as *const u32);
        let chunk_start = ahf + 0x4;
        core::ptr::write_volatile(ahf as *mut u32, requested_size);
        core::ptr::write_volatile(MEM_BLOCK_START as *mut u32, chunk_start + requested_size);
        return chunk_start;
        // 0x666
    } else {
        let ahf = core::ptr::read_volatile(MEM_BLOCK_START as *const u32);
        let chunk_start = ahf + 0x4;
        core::ptr::write_volatile(ahf as *mut u32, requested_size);
        core::ptr::write_volatile(MEM_BLOCK_START as *mut u32, chunk_start + requested_size);
        return chunk_start;
    }
}

pub fn free(addr: u32) {
    unsafe {
        core::ptr::write_volatile(
            (addr - 0x04) as *mut u32,
            core::ptr::read_volatile((addr - 0x4) as *const u32),
        );
        let num_of_journal_entries = core::ptr::read_volatile(JOURNAL_NUM_OF_ELEMENTS as *mut u32);

        core::ptr::write_volatile(
            JOURNAL_NUM_OF_ELEMENTS as *mut u32,
            num_of_journal_entries + 1,
        );
        core::ptr::write_volatile(
            (JOURNAL_START + num_of_journal_entries * 0x04) as *mut u32,
            addr - 0x04,
        );
    }
}

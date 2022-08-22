const JOURNAL_BASE: u32 = 0x2000_0000;
const JOURNAL_NUM_OF_ELEMENTS: u32 = JOURNAL_BASE;
const JOURNAL_START: u32 = JOURNAL_BASE + 0x04;

const MEM_BLOCK_START: u32 = 0x2000_0048;
const ADDR_OF_HIGHEST_FREE_BLOCK: u32 = MEM_BLOCK_START + 0x04;
const WORD: u32 = 0x4;

use core::intrinsics::{volatile_load, volatile_store};
use core::ptr::swap;

pub unsafe fn init() {
    volatile_store(MEM_BLOCK_START as *mut u32, ADDR_OF_HIGHEST_FREE_BLOCK);
    volatile_store(JOURNAL_NUM_OF_ELEMENTS as *mut u32, 0x0000_0000);
}

pub unsafe fn get_mem(requested_size: u32) -> u32 {
    let mut no_gap_found = true;
    if volatile_load(JOURNAL_NUM_OF_ELEMENTS as *const u32) != 0 {
        // first fit algorithm
        let entries = volatile_load(JOURNAL_NUM_OF_ELEMENTS as *const u32);
        for journal_entry in 0..entries {
            let journal_entry_addr = JOURNAL_START + journal_entry * WORD;
            let free_entry = volatile_load(journal_entry_addr as *const u32);
            let size_available = volatile_load(free_entry as *const u32);
            if requested_size <= size_available {
                volatile_store(journal_entry_addr as *mut u32, 0x0000_0000);

                let journal_size = volatile_load(JOURNAL_NUM_OF_ELEMENTS as *const u32);
                let end_adress = JOURNAL_BASE + journal_size * WORD;

                if journal_size == 1 {
                    volatile_store(journal_entry_addr as *mut u32, 0x0000_0000);
                } else {
                    swap(end_adress as *mut u32, journal_entry_addr as *mut u32);
                }
                volatile_store(
                    JOURNAL_NUM_OF_ELEMENTS as *mut u32,
                    volatile_load(JOURNAL_NUM_OF_ELEMENTS as *const u32) - 1,
                );
                volatile_store(free_entry as *mut u32, requested_size);
                return free_entry + WORD;
            }
        }
        no_gap_found = true;
    }

    if volatile_load(JOURNAL_NUM_OF_ELEMENTS as *const u32) == 0 || no_gap_found {
        let ahf = volatile_load(MEM_BLOCK_START as *const u32);
        let chunk_start = ahf + WORD;
        volatile_store(ahf as *mut u32, requested_size);
        volatile_store(MEM_BLOCK_START as *mut u32, chunk_start + requested_size);
        return chunk_start;
    }
    0
}

pub fn free(addr: u32) {
    unsafe {
        volatile_store(
            (addr - WORD) as *mut u32,
            volatile_load((addr - 0x4) as *const u32),
        );
        let num_of_journal_entries = volatile_load(JOURNAL_NUM_OF_ELEMENTS as *mut u32);

        volatile_store(
            JOURNAL_NUM_OF_ELEMENTS as *mut u32,
            num_of_journal_entries + 1,
        );
        volatile_store(
            (JOURNAL_START + num_of_journal_entries * WORD) as *mut u32,
            addr - WORD,
        );
    }
}

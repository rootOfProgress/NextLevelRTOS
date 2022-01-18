const JOURNAL_BASE: u32 = 0x2000_0000;
const JOURNAL_NUM_OF_ELEMENTS: u32 = JOURNAL_BASE;
const JOURNAL_START: u32 = JOURNAL_BASE + 0x04;

const MEM_BLOCK_START: u32 = 0x2000_0048;
const ADDR_OF_HIGHEST_FREE_BLOCK: u32 = MEM_BLOCK_START + 0x04;
const WORD: u32 = 0x4;
use core::ptr::{read_volatile, write_volatile, swap};

pub unsafe fn init() {
    write_volatile(MEM_BLOCK_START as *mut u32, ADDR_OF_HIGHEST_FREE_BLOCK);
    write_volatile(JOURNAL_NUM_OF_ELEMENTS as *mut u32, 0x0000_0000);
}

pub unsafe fn get_mem(requested_size: u32) -> u32 {
    let mut no_gap_found = true;
    if read_volatile(JOURNAL_NUM_OF_ELEMENTS as *const u32) != 0 {
        // first fit algorithm
        let entries = read_volatile(JOURNAL_NUM_OF_ELEMENTS as *const u32);
        for journal_entry in 0..entries {
            let journal_entry_addr = JOURNAL_START + journal_entry * WORD;
            let free_entry = read_volatile(journal_entry_addr as *const u32);
            let size_available = read_volatile(free_entry as *const u32);
            if requested_size <= size_available {
                write_volatile(journal_entry_addr as *mut u32, 0x0000_0000);

                let journal_size = read_volatile(JOURNAL_NUM_OF_ELEMENTS as *const u32);
                let end_adress = JOURNAL_BASE + journal_size * WORD;

                if journal_size == 1 {
                    write_volatile(journal_entry_addr as *mut u32, 0x0000_0000);
                } else {
                    swap(end_adress as *mut u32, journal_entry_addr as *mut u32);
                }
                write_volatile(
                    JOURNAL_NUM_OF_ELEMENTS as *mut u32,
                    read_volatile(JOURNAL_NUM_OF_ELEMENTS as *const u32) - 1,
                );
                write_volatile(free_entry as *mut u32, requested_size);
                return free_entry + WORD;
            }
        }
        no_gap_found = true;
    } 
    
    if read_volatile(JOURNAL_NUM_OF_ELEMENTS as *const u32) == 0 || no_gap_found {
        let ahf = read_volatile(MEM_BLOCK_START as *const u32);
        let chunk_start = ahf + WORD;
        write_volatile(ahf as *mut u32, requested_size);
        write_volatile(MEM_BLOCK_START as *mut u32, chunk_start + requested_size);
        return chunk_start;
    }
    0
}

pub fn free(addr: u32) {
    unsafe {
        write_volatile(
            (addr - WORD) as *mut u32,
            read_volatile((addr - 0x4) as *const u32),
        );
        let num_of_journal_entries = read_volatile(JOURNAL_NUM_OF_ELEMENTS as *mut u32);

        write_volatile(
            JOURNAL_NUM_OF_ELEMENTS as *mut u32,
            num_of_journal_entries + 1,
        );
        write_volatile(
            (JOURNAL_START + num_of_journal_entries * WORD) as *mut u32,
            addr - WORD,
        );
    }
}

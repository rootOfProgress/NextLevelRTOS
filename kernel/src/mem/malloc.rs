// use core::alloc::Layout;
const META: u32 = 0x20000044;
const MEM_START: u32 = 0x20000048;

pub unsafe fn init() {
    core::ptr::write_volatile(META as *mut u32, MEM_START);
}

pub fn get_mem(size: u32) -> u32 {
    let start = unsafe { core::ptr::read_volatile(META as *mut u32) };

    // let current_bound = unsafe { core::ptr::read_volatile(meta as *mut u32) };
    let next_bound = start + size;
    unsafe {
        core::ptr::write_volatile(META as *mut u32, next_bound);
    };
    start
}

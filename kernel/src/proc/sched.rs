//---------------------------------------------------------------//
//----------------------------IMPORTS----------------------------//
//---------------------------------------------------------------//
use super::super::data::list::List;
use super::task;
use super::tcb::TCB;
use process::blueprint::Frame;
static mut TASK_LIST_ADDR: u32 = 0;

extern "C" {
    ///
    /// Points to extern asm instruction.
    ///
    fn __br_to_init();
    fn __trap();
    fn __load_process_context(stack_addr: u32);
    fn __save_process_context() -> u32;
}

pub fn init() {
    unsafe { TASK_LIST_ADDR = List::new() };
    // unsafe { TASK_LIST = list::List::new() };
}

pub fn destroy() {}

pub fn shift_task() -> u32 {
    let list = unsafe { &mut *(TASK_LIST_ADDR as *mut List) };
    list.sr_cursor()
}

pub fn start_init_process() {
    let i = unsafe { &mut *(shift_task() as *mut TCB) };
    unsafe {
        __load_process_context(i.sp);
        core::ptr::write_volatile(
            0xE000_E010 as *mut u32,
            core::ptr::read_volatile(0xE000_E010 as *const u32) | 0b1,
        );
        __trap();
    }
}

pub fn spawn(p: Frame) {
    let list = unsafe { &mut *(TASK_LIST_ADDR as *mut List) };
    let r4 = p.get_r4_location();
    let tcb = task::create(r4);
    list.insert(tcb);
}

pub fn context_switch() {
    unsafe {
        let old_sp = __save_process_context();
        let list = &mut *(TASK_LIST_ADDR as *mut List);
        list.update_tcb(old_sp);
        let tcb = &mut *(shift_task() as *mut TCB);
        __load_process_context(tcb.sp);
    }
}

#[no_mangle]
pub extern "C" fn SysTick() {
    unsafe { asm!("bkpt") };
    context_switch();
}

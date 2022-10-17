//---------------------------------------------------------------//
//----------------------------IMPORTS----------------------------//
//---------------------------------------------------------------//
use super::super::data::list::List;
use super::task::task::Frame;
use super::tcb::TCB;
use core::intrinsics::{volatile_load, volatile_store};
static mut TASK_LIST_ADDR: u32 = 0;

extern "C" {
    ///
    /// Points to extern asm instruction.
    ///
    // fn __br_to(sp: u32);
    fn __br_to_init();
    fn __trap(f: u32);
    fn __load_process_context(stack_addr: u32);
    fn __save_process_context() -> u32;
}

pub fn init() {
    unsafe { TASK_LIST_ADDR = List::new() };
    // unsafe { TASK_LIST = list::List::new() };
}

pub fn destroy() {
    unsafe {
        core::intrinsics::volatile_store(
            0xE000_E010 as *mut u32,
            core::ptr::read_volatile(0xE000_E010 as *const u32) & !0b1,
        );
        // core::ptr::write_volatile(
        // 0xE000_E010 as *mut u32,
        // core::ptr::read_volatile(0xE000_E010 as *const u32) & !0b1,
        // );
        let list = &mut *(TASK_LIST_ADDR as *mut List);
        list.delete_head_node();
        let p = list.cursor_sp();

        core::ptr::write_volatile(
            0xE000_E010 as *mut u32,
            core::ptr::read_volatile(0xE000_E010 as *const u32) | 0b1,
        );
        __trap(p);
    }
}

pub fn shift_task() -> u32 {
    let list = unsafe { &mut *(TASK_LIST_ADDR as *mut List) };
    list.sr_cursor_sp()
}

pub fn start_init_process() {
    unsafe {
        let p = shift_task();
        // __load_process_context(shift_task());
        // core::ptr::write_volatile(
        //     0xE000_E010 as *mut u32,
        //     core::ptr::read_volatile(0xE000_E010 as *const u32) | 0b1,
        // );
        __trap(p);
    }
}

pub fn spawn(pid: u32, p: Frame, name: &str) {
    let list = unsafe { &mut *(TASK_LIST_ADDR as *mut List) };
    let r4 = p.get_r4_location();
    let tcb = TCB::create(r4, pid, name);
    list.insert(tcb);
}

pub fn context_switch() {
    unsafe {
        let old_sp = __save_process_context();
        let list = &mut *(TASK_LIST_ADDR as *mut List);
        list.update_tcb_and_shift_task(old_sp);
        // list.update_tcb(old_sp);
        let sp = list.sr_cursor_sp();

        __load_process_context(sp);
    }
}

#[no_mangle]
pub extern "C" fn SysTick() {
    unsafe {
        // turn on PA1

        volatile_store(
            0x4800_0014 as *mut u32,
            volatile_load(0x4800_0014 as *const u32) | 1 << 1,
        );

        volatile_store(
            0x4800_0014 as *mut u32,
            volatile_load(0x4800_0014 as *const u32) & !1,
        );

        // turn off PA1
        volatile_store(
            0x4800_0014 as *mut u32,
            volatile_load(0x4800_0014 as *const u32) & !(1 << 1),
        );

        // turn on PA0
        volatile_store(
            0x4800_0014 as *mut u32,
            volatile_load(0x4800_0014 as *const u32) | 1,
        );

        //context_switch();
        let old_sp = __save_process_context();
        // let list = &mut *(TASK_LIST_ADDR as *mut List);
        let sp = (&mut *(TASK_LIST_ADDR as *mut List)).update_tcb_and_shift_task(old_sp);
        // let sp = list.update_tcb_and_shift_task(old_sp);
        // list.update_tcb(old_sp);
        // let sp = list.sr_cursor_sp();

        __load_process_context(sp);
        // turn on PA0
        volatile_store(
            0x4800_0014 as *mut u32,
            volatile_load(0x4800_0014 as *const u32) & !1,
        );
    }
}

#[no_mangle]
pub extern "C" fn SVCall() {
    unsafe { __br_to_init() };
}

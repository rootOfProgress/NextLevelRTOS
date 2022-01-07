//---------------------------------------------------------------//
//----------------------------IMPORTS----------------------------//
//---------------------------------------------------------------//
use super::super::data::list;
use super::task;
use core::sync::atomic::{AtomicU32, Ordering};
use process::blueprint::Frame;
// static NUM_OF_TASKS: AtomicU32 = AtomicU32::new(0);
static mut CURRENT_TASK_TCB: u32 = 0;
static mut TASK_LIST_ADDR: u32 = 0;
static mut TASK_LIST_ADADR: Option<list::List> = None;

extern "C" {
    ///
    /// Points to extern asm instruction.
    ///
    fn __br_to_init();
    fn __trap();
    fn __load_process_context(stack_addr: u32);
}

pub fn init() {
    unsafe { TASK_LIST_ADDR = list::List::new() };
}

pub fn destroy() {}

pub fn next_task() -> u32 {
    let list = unsafe { &mut *(TASK_LIST_ADDR as *mut list::List) };
    list.next()
}

pub fn spawn(mut p: Frame) {
    let list = unsafe { &mut *(TASK_LIST_ADDR as *mut list::List) };
    let r4 = p.get_r4_location();
    let tcb = task::create(r4);
    list.insert(tcb);
    // match process {
    //     Some(p) => {
    //         let r4 = p.get_r4_location();
    //         unsafe{asm!("bkpt");}
    //     },
    //     None => {/*assert kernel panic*/}
    // }

    // let pid = task::insert(p.get_frame_size(), r4, NUM_OF_TASKS.load(Ordering::Relaxed));

    // if pid == 1 {
    //     unsafe {
    //         __load_process_context(r4);
    //         __trap();
    //     }
    // } else {
    //     // activate scheduler
    // }
}

pub fn run(task: u32) {
    unsafe {
        __load_process_context(task);
        __trap();
    }
}

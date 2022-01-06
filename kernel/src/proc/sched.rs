//---------------------------------------------------------------//
//----------------------------IMPORTS----------------------------//
//---------------------------------------------------------------//
use process::blueprint::Frame;
use core::sync::atomic::{AtomicU32, Ordering};
use super::task;
static NUM_OF_TASKS: AtomicU32 = AtomicU32::new(0);

extern "C" {
    ///
    /// Points to extern asm instruction.
    ///
    fn __br_to_init();
    fn __trap();
    fn __load_process_context(stack_addr: u32);
}

pub fn destroy() {

}

pub fn spawn(mut p: Frame) {
    // match process {
    //     Some(p) => {
    //         let r4 = p.get_r4_location();
    //         unsafe{asm!("bkpt");}
    //     },
    //     None => {/*assert kernel panic*/}
    // }
    let r4 = p.get_r4_location();
    
    let pid = task::insert(p.get_frame_size(), r4, NUM_OF_TASKS.load(Ordering::Relaxed));

    if pid == 1 {
        unsafe {
            __load_process_context(r4);
            __trap();
        }
    } else {
        // activate scheduler
    }
}

//---------------------------------------------------------------//
//----------------------------IMPORTS----------------------------//
//---------------------------------------------------------------//
use process::blueprint::Frame;

extern "C" {
    ///
    /// Points to extern asm instruction.
    ///
    fn __br_to_init();
    fn __trap();
    fn __load_process_context(stack_addr: u32);
}

pub fn spawn(p: Frame) {
    // match process {
    //     Some(p) => {
    //         let r4 = p.get_r4_location();
    //         unsafe{asm!("bkpt");}
    //     },
    //     None => {/*assert kernel panic*/}
    // }
    let r4 = p.get_r4_location();

    unsafe {
        __load_process_context(r4);
        __trap();
    }
}

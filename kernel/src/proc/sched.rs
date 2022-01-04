use process::blueprint::Frame;

pub fn spawn(p: Frame) {
    // match process {
    //     Some(p) => {
    //         let r4 = p.get_r4_location();
    //         unsafe{asm!("bkpt");}
    //     },
    //     None => {/*assert kernel panic*/}
    // }
    let r4 = p.get_r4_location();
    unsafe{asm!("bkpt");}
} 
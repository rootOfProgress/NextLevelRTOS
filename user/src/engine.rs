extern crate devices;
use devices::controller::timer::tim::TimerDevice;

use devices::controller::uart::res;

pub fn alter_engine_speed(mut timer_device: Option<TimerDevice>, engine_number: u32) {
    let mut t: TimerDevice = match timer_device {
        Some(ref mut x) => *x,
        None => panic!(),
    };

    unsafe {
        match engine_number {
            1 => t = t.set_ccr1_register(res[0]),
            2 => t = t.set_ccr2_register(res[1]),
            3 => t = t.set_ccr3_register(res[2]),
            4 => t = t.set_ccr4_register(res[3]),
            _ => panic!(),
        }
    }
}

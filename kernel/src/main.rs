//!
//! Main crate for Kernelimplementation. Depends on runtime
//! and the main function gets called after successful power on.
//!
#![no_std]
#![no_main]
#![feature(asm)]
extern crate devices;
extern crate process;
extern crate runtime;

mod proc;

use proc::sched::spawn;

fn user_init() {
    loop {}
}

///
/// Target function after hardware initialization,
/// acts as the first kernel function.
///
#[no_mangle]
pub unsafe fn kernel_init() -> ! {
    let gpio_port_a1 = devices::io::gpio::gpio::GpioDevice::new("A", 1)
        .as_output()
        .as_push_pull();
    gpio_port_a1.turn_on();

    let gpio_port_e14 = devices::io::gpio::gpio::GpioDevice::new("E", 14)
        .as_output()
        .as_push_pull();
    gpio_port_e14.turn_on();

    let early_user_land = process::new_process(user_init as *const () as u32).unwrap();

    spawn(early_user_land);
    // let y = process::new_process(0xABCD_EBEB).unwrap();
    // spawn(y);
    loop {}
}

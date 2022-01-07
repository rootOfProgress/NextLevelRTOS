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
mod data;
mod mem;
mod proc;
use devices::io::gpio::gpio::GpioDevice;

use proc::sched;

fn quix() {
    loop {}
}

fn quax() {
    loop {}
}

fn bar() {
    loop {}
}

fn user_init() {

    let foo = process::new_process(bar as *const () as u32).unwrap();
    let quix = process::new_process(quix as *const () as u32).unwrap();
    let quax = process::new_process(quax as *const () as u32).unwrap();
    sched::spawn(foo);
    sched::spawn(quix);
    sched::spawn(quax);
    loop {}
}
///
/// Target function after hardware initialization,
/// acts as the first kernel function.
///
#[no_mangle]
pub unsafe fn kernel_init() -> ! {
    mem::malloc::init();
    sched::init();

    let gpio_port_e14 = GpioDevice::new("E", 14).as_output().as_push_pull();
    gpio_port_e14.turn_on();

    GpioDevice::new("A", 9)
        .as_alternate_function()
        .as_push_pull()
        .as_af(7);

    let usart = devices::controller::uart::usart::UsartDevice::new(9600);
    usart.enable();
    usart.print_str("hello world!\n\r");

    let early_user_land = process::new_process(user_init as *const () as u32).unwrap();

    usart.print_str("spawn userland!\n\r");
    sched::spawn(early_user_land);

    // activate systick here
    devices::sys::tick::init_systick(1);
    // devices::sys::tick::enable_systick();
    sched::start_init_process();

    // devices::sys::tick::init_systick(1000);
    // devices::sys::tick::enable_systick();

    loop {
        usart.print_str("!! KERNEL PANIC - NO INIT FOUND !!\n\r");
        asm!("bkpt");
    }
}

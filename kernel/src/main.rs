//!
//! Main crate for Kernelimplementation. Depends on runtime
//! and the main function gets called after successful power on.
//!
#![no_std]
#![no_main]

extern crate devices;
extern crate runtime;

///
/// Target function after hardware initialization,
/// acts as the first kernel function.
///
#[no_mangle]
pub unsafe fn kernel_init() -> ! {
    let gpio_port_a2 = devices::io::gpio::gpio::GpioDevice::new("A", 2)
        .as_output()
        .as_push_pull();
    gpio_port_a2.turn_on();

    let gpio_port_a3 = devices::io::gpio::gpio::GpioDevice::new("E", 3)
        .as_output()
        .as_push_pull();
    gpio_port_a3.turn_on();

    loop {}
}

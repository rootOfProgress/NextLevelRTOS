//!
//! Main crate for Kernelimplementation. Depends on lake_rtos_rt
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
pub fn kernel_init() -> ! {

    let gpio_port_a1 = devices::base::gpio::GpioPort::new("A", 1)
        .as_output()
        .as_push_pull();
    gpio_port_a1.turn_on();

    let gpio_port_e14 = devices::base::gpio::GpioPort::new("E", 14)
        .as_output()
        .as_push_pull();
    gpio_port_e14.turn_on();

    loop {}
}

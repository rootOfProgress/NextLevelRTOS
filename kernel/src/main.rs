//!
//! Main crate for Kernelimplementation. Depends on runtime
//! and the main function gets called after successful power on.
//!
#![no_std]
#![no_main]
#![feature(asm)]
#![feature(core_intrinsics)]
extern crate devices;
extern crate process;
extern crate runtime;
mod data;
mod mem;
mod proc;
use devices::controller::uart::iostream;
use devices::generic::platform::stm32f407ve::adresses;
use devices::io::gpio::gpio::GpioDevice;
use proc::sched;

fn fibonacci(n: u32) -> u32 {
    match n {
        0 => 1,
        1 => 1,
        _ => fibonacci(n - 1) + fibonacci(n - 2),
    }
}

fn quix() {
    loop {
        unsafe {
            let mut reg_content = core::ptr::read_volatile(0x4002_0014 as *mut u32);
            reg_content &= !((0b1_u32) << 2);
            core::ptr::write_volatile(0x4002_0014 as *mut u32, reg_content);
        }
    }
}

fn quax() {
    loop {
        unsafe {
            let mut reg_content = core::ptr::read_volatile(0x4002_0014 as *mut u32);
            reg_content |= (0b1_u32) << 2;
            core::ptr::write_volatile(0x4002_0014 as *mut u32, reg_content);
        }
    }
}

fn bar() {
    // loop {
    fibonacci(22);
    // }
}

fn user_init() {
    let bar =
        process::new_process(bar as *const () as u32, sched::destroy as *const () as u32).unwrap();
    let quix =
        process::new_process(quix as *const () as u32, sched::destroy as *const () as u32).unwrap();
    let quax =
        process::new_process(quax as *const () as u32, sched::destroy as *const () as u32).unwrap();
    //"spawn process 1".println();
    //sched::spawn(1, bar, "bar");
    "spawn process 2".println();
    sched::spawn(2, quix, "quix");
    "spawn process 3".println();
    sched::spawn(3, quax, "quax");
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
    devices::sys::tick::init_systick(280);

    let gpio_port_a2 = devices::io::gpio::gpio::GpioDevice::new("A", 2)
        .as_output()
        .as_push_pull();
    gpio_port_a2.turn_on();

    let gpio_port_a3 = devices::io::gpio::gpio::GpioDevice::new("A", 3)
        .as_output()
        .as_push_pull();
    gpio_port_a3.turn_on();

    GpioDevice::new("A", 9)
        .as_alternate_function()
        .as_push_pull()
        .as_af(7);

    let usart = devices::controller::uart::usart::UsartDevice::new(9600);
    usart.enable();

    let early_user_land =
        process::new_process(user_init as *const () as u32, user_init as *const () as u32).unwrap();

    "hello from trait".println();
    "usart works without errors...".println();
    sched::spawn(0, early_user_land, "early_user_land");
    sched::start_init_process();

    loop {
        "!! KERNEL PANIC - NO INIT FOUND !!".println();
        asm!("bkpt");
    }
}

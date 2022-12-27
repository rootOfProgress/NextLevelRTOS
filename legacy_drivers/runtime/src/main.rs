//!
//! Contains the first entrypoint for startup procedure.
//! Only for runtime purposes, acts as a platform for the kernel
//! code build on top of it.
//!
#![no_std]
#![no_main]
#![feature(rustc_private)]
// #![no_main]
use core::panic::PanicInfo;
extern crate devices;

use devices::init_devs;
// use devices;

// extern crae libc;
#[no_mangle]
#[link_section = ".main"]
pub extern "C" fn main() {
    unsafe {
        init_devs();
    }
    // loop{}
}

#[panic_handler]
fn panic(_panic: &PanicInfo<'_>) -> ! {
    loop {}
}

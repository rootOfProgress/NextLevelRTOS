//!
//! Contains the first entrypoint for startup procedure.
//! Only for runtime purposes, acts as a platform for the kernel
//! code build on top of it.
//!
#![no_std]
#![no_main]
#![feature(rustc_private)]
use core::panic::PanicInfo;
extern crate devices;

use devices::init_devs;
#[no_mangle]
#[link_section = ".main"]
pub extern "C" fn main() {
    unsafe {
        init_devs();
    }
    // loop{}
}

#[panic_handler]
#[link_section = ".panic"]
fn panic(_panic: &PanicInfo<'_>) -> ! {
    loop {}
}

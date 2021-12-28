//!
//! Main crate for Kernelimplementation. Depends on runtime
//! and the main function gets called after successful power on. 
//!
#![no_std]
#![no_main]

extern crate runtime;

///
/// Target function after hardware initialization, 
/// acts as the first kernel function.
/// 
#[no_mangle]
pub fn kernel_init() -> ! {
    loop {}
}
#![no_main]
#![no_std]

use core::panic::PanicInfo;

#[panic_handler]
fn panic(_panic: &PanicInfo<'_>) -> ! {
    loop {}
}

// extern fn panic_fmt(_: ::core::fmt::Arguments, _: &'static str, _: u32) -> ! {
//     loop {}
// }

#[no_mangle]
fn _ZN4core9panicking5panic17hf95d14f8e84b22e4E()
{

}

#[no_mangle]
fn entry(t: u32) -> u32 {
    let i: u32 = 1;
    if t > 42 {
        return i + 2
    }
    i 
}

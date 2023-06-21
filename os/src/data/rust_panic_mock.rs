
#![no_main]
#![no_std]

use core::panic::PanicInfo;

#[panic_handler]
fn panic(_panic: &PanicInfo<'_>) -> ! {
    loop {}
}
#[no_mangle]
fn _ZN4core9panicking5panic17hf95d14f8e84b22e4E()
{

}

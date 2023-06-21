#![no_main]
#![no_std]

mod rust_panic;

#[no_mangle]
fn entry(t: u32) -> u32 {
    let i: u32 = 1;
    if t > 42 {
        return i + 2
    }
    386
}

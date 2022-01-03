//!
//! Main crate for Kernelimplementation. Depends on runtime
//! and the main function gets called after successful power on.
//!
#![no_std]
#![no_main]
#![feature(asm)]
extern crate devices;
extern crate runtime;

use devices::generic::platform::stm32f3x::{adresses, offsets};
///
/// Target function after hardware initialization,
/// acts as the first kernel function.
///
#[no_mangle]
pub unsafe fn kernel_init() -> ! {
    let gpio_port_a1 = devices::io::gpio::gpio::GpioDevice::new("A", 1)
        .as_output()
        .as_push_pull();
    gpio_port_a1.turn_on();

    let gpio_port_e14 = devices::io::gpio::gpio::GpioDevice::new("E", 14)
        .as_output()
        .as_push_pull();
    gpio_port_e14.turn_on();

    let usgp = devices::io::gpio::gpio::GpioDevice::new("A", 9)
        .as_alternate_function()
        .as_push_pull()
        .as_af(7);

    let usi = devices::controller::uart::usart::UsartDevice::new(9600);
    usi.enable();
    // usi.print_str("morgeeeen\n\r");
    // asm!("bkpt");
    let mystr = "hello dma".as_bytes();
    let bar = core::ptr::addr_of!(mystr[0]);
    let dma = devices::controller::dma::dma::DmaDevice::new(4).disable();
    dma.memory_is_source()
        .peripherial_target_addr(adresses::USART1_BASEADRESS | offsets::usart1::TDR)
        .mem_target_addr((bar as *const u32) as u32)
        .transfer_amount(8)
        .enable();
    asm!("bkpt");

    loop {}
}

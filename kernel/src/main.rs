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
    usi.dma_tx().enable();
    // usi.print_str("morgeeeen\n\r");
    // asm!("bkpt");
    let mystr = "hello dma";
    let bar = core::ptr::addr_of!(mystr);
    let mut dma = devices::controller::dma::dma::DmaDevice::new(4).disable();
    
    for i in (0x2000_0200..0x2000_0500).step_by(0x04) {
        core::ptr::write_volatile(i as *mut u32, 0x5859_5859);
    }
    asm!("bkpt");
    dma = dma.mem_size(8)
        .periph_size(8)
        .memory_is_source()
        .peripherial_target_addr(adresses::USART1_BASEADRESS | offsets::usart1::TDR)
        .mem_target_addr(0x2000_0300)
        .transfer_amount(65535)
        .enable();

    dma.disable()
        .mem_target_addr(0x2000_0500)
        .transfer_amount(600)
        .enable();
    // asm!("bkpt");

    loop {}
}

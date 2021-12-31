//!
//! Main crate for Kernelimplementation. Depends on runtime
//! and the main function gets called after successful power on.
//!
#![no_std]
#![no_main]

extern crate devices;
extern crate runtime;

use devices::controller::dma::dma::*;
use devices::generic::platform::stm32f3x::*;
use core::ptr::{write_volatile, read_volatile};
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

    let hello: &str = "hello dma!";

    let usart1 = devices::controller::uart::uart::new(1, 9600);
    usart1.enable();
    
    devices::controller::uart::uart::print_str(hello);

    // disable_dma_channel();

    // // Write the USART_TDR register address in the DMA control register to configure it as
    // //the destination of the transfer. The data is moved to this address from memory after
    // //each TXE event.
    // write_peripherial_adress(adresses::USART1_BASEADRESS | offsets::usart1::TDR);

    // //Write the memory address in the DMA control register to configure it as the source of
    // //the transfer. The data is loaded into the USART_TDR register from this memory area
    // //after each TXE event.
    // write_memory_adress(hello.as_ptr() as u32);

    // // Configure the total number of bytes to be transferred to the DMA control register.
    // set_number_of_bytes_to_process(7);

    // // Clear the TC flag in the USART_ISR register by setting the TCCF bit in the
    // //USART_ICR register.
    // unsafe {
    //     write_volatile((adresses::USART1_BASEADRESS | offsets::usart1::ICR) as *mut u32 , read_volatile((adresses::USART1_BASEADRESS | offsets::usart1::ICR) as *const u32 ) | bitfields::usart1::TCCF);
    // }

    // // Activate the channel in the DMA register.
    // enable_dma_channel();

    loop {}
}

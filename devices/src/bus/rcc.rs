//!
//! This module provides basic access to the clock bus
//! system. It provides different functions to
//! seperate the activation for the devices actually needed.
//!
use super::super::generic::platform::stm32f3x;
use super::super::registerblocks::rcc::RCC;

// const foo: &'static RCC = RCC::new(123);

pub mod rcc {
    use crate::generic::platform::stm32f3x::bitfields::rcc;

    //---------------------------------------------------------------//
    //----------------------------IMPORTS----------------------------//
    //---------------------------------------------------------------//
    use super::stm32f3x::adresses;
    use super::stm32f3x::bitfields;
    use super::stm32f3x::offsets;

    use super::RCC;
    use core::ptr;

    ///
    /// Activates the GPIO Clock for the given Port Name .
    /// 
    /// # Arguments
    /// 
    /// * `port_mnemonic` - A string that describes the port name, e.g. "A" .
    ///
    /// # Returns
    /// * `None`
    /// 
    pub fn activate_gpio_bus_clock(port_mnemonic: &str) {

        // temporary value
        let rcc_bus = RCC::new(adresses::RCC);
        unsafe {
            match port_mnemonic {
                "A" => ptr::write((*(rcc_bus)).ahbenr as *mut u32, ptr::read((*(rcc_bus)).ahbenr as *mut u32) | 0b1 << bitfields::rcc::IOPAEN),
                "E" => ptr::write((*(rcc_bus)).ahbenr as *mut u32, ptr::read((*(rcc_bus)).ahbenr as *mut u32) | 0b1 << bitfields::rcc::IOPEEN),
                _ => {}
            };
        }
    }
}

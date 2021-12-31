//!
//! This module provides basic access to the clock bus
//! system. It provides different functions to
//! seperate the activation for the devices actually needed.
//!
use super::generic::platform::stm32f3x;

pub mod rcc {
    //---------------------------------------------------------------//
    //----------------------------IMPORTS----------------------------//
    //---------------------------------------------------------------//
    use super::stm32f3x::adresses;
    use super::stm32f3x::bitfields;
    use super::stm32f3x::offsets;

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
        unsafe {
            match port_mnemonic {
                "A" => ptr::write((adresses::RCC | offsets::rcc::RCC_AHBENR) as *mut u32, ptr::read((adresses::RCC | offsets::rcc::RCC_AHBENR) as *mut u32) | 0b1 << bitfields::rcc::IOPAEN),
                "E" => ptr::write((adresses::RCC | offsets::rcc::RCC_AHBENR) as *mut u32, ptr::read((adresses::RCC | offsets::rcc::RCC_AHBENR) as *mut u32) | 0b1 << bitfields::rcc::IOPEEN),
                _ => {}
            };
        }
    }

    pub fn activate_usart_bus_clock() {
        unsafe {
            let rcc_apb2enr: u32 = adresses::RCC | offsets::rcc::RCC_APB2ENR;
            ptr::write_volatile(rcc_apb2enr as *mut u32, ptr::read_volatile(rcc_apb2enr as *const u32) | ((0b1 << bitfields::rcc::USART1EN) | bitfields::rcc::SYSCFGEN));
        }
    }
}

//!
//! This module provides basic access to the clock bus
//! system. It provides different functions to
//! seperate the activation for the devices actually needed.
//!

//---------------------------------------------------------------//
//----------------------------IMPORTS----------------------------//
//---------------------------------------------------------------//
use super::super::generic::platform::stm32f3x;
use super::super::generic::traits::primitive_extensions;
use super::super::registerblocks::rcc::RCC;
pub mod rcc {

    //---------------------------------------------------------------//
    //-------------------------LOCAL-IMPORTS-------------------------//
    //---------------------------------------------------------------//
    use super::stm32f3x::adresses;
    use super::stm32f3x::bitfields;
    use super::primitive_extensions::BitOps;

    use super::RCC;

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
    pub unsafe fn activate_gpio_bus_clock(port_mnemonic: &str) {
        let rcc_bus = RCC::new(adresses::RCC);
        match port_mnemonic {
            "A" => (*rcc_bus).ahbenr.set_bit(0b1 << bitfields::rcc::IOPAEN),
            "E" => (*rcc_bus).ahbenr.set_bit(0b1 << bitfields::rcc::IOPEEN),
            _ => {}
        };
    }

    pub unsafe fn activate_usart1_bus_clock() {
        let rcc_bus = RCC::new(adresses::RCC);
        (*rcc_bus).apb2enr.set_bit((0b1 << bitfields::rcc::USART1EN) | bitfields::rcc::SYSCFGEN);
    }

    pub unsafe fn activate_dma1_bus_clock() {
        let rcc_bus = RCC::new(adresses::RCC);
        (*rcc_bus).ahbenr.set_bit(0b1);
    }
}

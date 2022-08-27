//!
//! This module provides basic access to the gpio registers.
//! The configuration of the ports can easily be done with
//! a provided builder.
//!
use super::super::bus::rcc;
use super::super::generic::platform::stm32f3x;
use super::super::generic::traits::primitive_extensions;
use super::super::registerblocks::i2c::I2C;

pub mod i2c {
    //---------------------------------------------------------------//
    //----------------------------IMPORTS----------------------------//
    //---------------------------------------------------------------//
    use super::primitive_extensions::BitOps;
    use super::rcc;
    use super::stm32f3x::adresses;
    use super::stm32f3x::bitfields;
    use super::I2C;


    //---------------------------------------------------------------//
    //-----------------------STRUCT-DEFINITONS-----------------------//
    //---------------------------------------------------------------//
    pub struct I2cDevice {
        device: &'static I2C,
    }

    //---------------------------------------------------------------//
    //---------------------STRUCT-IMPLEMENTATIONS--------------------//
    //---------------------------------------------------------------//
    impl I2cDevice {
        pub unsafe fn new() -> I2cDevice {
            rcc::rcc::activate_i2c_bus_clock();

            I2cDevice {
                device: I2C::new(adresses::I2C1_BASEADRESS),
            }
        }

        pub unsafe fn init(self) -> I2cDevice {
            self.set_timing2_register(0x2000090E);
            self.set_cr1_register(1 << bitfields::i2c::PE);
            self
            // self.set
        } 

        unsafe fn set_cr1_register(&self, value: u32) {
            self.device.CR1.set_bit(value);
        }

        unsafe fn set_cr2_register(&self, value: u32) {
            self.device.CR2.set_bit(value);
        }

        unsafe fn set_oar1_register(&self, value: u32) {
            self.device.OAR1.set_bit(value);
        }

        unsafe fn set_oar2_register(&self, value: u32) {
            self.device.OAR2.set_bit(value);
        }

        unsafe fn set_timing2_register(&self, value: u32) {
            // self.device.TIMINGR.set_bit(value);
            self.device.TIMINGR.replace_whole_register(value);
        }

        unsafe fn set_timeoutr_register(&self, value: u32) {
            self.device.TIMEOUTR.set_bit(value);
        }

        unsafe fn set_isr_register(&self, value: u32) {
            self.device.ISR.set_bit(value);
        }

        pub unsafe fn set_icr_register(&self, value: u32) {
            self.device.ICR.set_bit(value);
        }

        unsafe fn set_pecr_register(&self, value: u32) {
            self.device.PECR.set_bit(value);
        }
    }
}

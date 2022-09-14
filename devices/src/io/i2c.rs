//!
//! This module provides basic access to the gpio registers.
//! The configuration of the ports can easily be done with
//! a provided builder.
//!
use super::super::bus::rcc;
use super::super::generic::platform::stm32f3x;
use super::super::generic::traits::primitive_extensions;
use super::super::registerblocks::i2c::I2C;
// use crate::io::i2c::i2c::I2cDevice;
pub mod i2c {
    //---------------------------------------------------------------//
    //----------------------------IMPORTS----------------------------//
    //---------------------------------------------------------------//
    use super::primitive_extensions::BitOps;
    use super::rcc;
    use super::stm32f3x::adresses;
    use super::stm32f3x::bitfields;
    use super::I2C;
    pub static mut I2C1_DEV: Option<I2cDevice> = None;

    //---------------------------------------------------------------//
    //-----------------------STRUCT-DEFINITONS-----------------------//
    //---------------------------------------------------------------//
    #[derive(Copy, Clone)]
    pub struct I2cDevice {
        device: &'static I2C,
    }

    pub fn get_i2c_dev() -> I2cDevice {
        unsafe {
            let mut t: I2cDevice = match I2C1_DEV {
                Some(ref mut x) => *x,
                None => panic!(),
            };
            t
        }
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
            self.set_oar1_register(0x1 << 1 | 1 << 15);
            self
            // self.set
        }

        pub unsafe fn write(self, data: u32, num_bytes: u32, slave_addr: u32) -> I2cDevice {
            self.set_cr2_register(
                slave_addr << bitfields::i2c::SADD_7_1
                    | num_bytes << bitfields::i2c::NBYTES
                    | 1 << bitfields::i2c::AUTOEND,
            );
            let mut tx_buffer: [u32; 4] = [0, 0, 0, 0];
            for byte in 0..num_bytes {
                // byte counts from 0 .. n
                let mut payload = data >> (8 * byte);

                // clear out the remaining 24 bit
                payload &= !(0xFF_FF_FF_00);
                tx_buffer[byte as usize] = payload;
            }
            self.start();
            for value in tx_buffer {
                self.set_txdr_register(value);
                while (self.get_isr() & (1 << 1) == 0) {}
            }

            self
        }

        pub unsafe fn start(self) -> I2cDevice {
            self.device.CR2.clear_bit(1 << bitfields::i2c::RD_WRN);
            self.device.CR2.set_bit(1 << bitfields::i2c::START);
            self
        }

        pub unsafe fn request_read(self) -> I2cDevice {
            // 10 = RD_WRN
            self.device.CR2.set_bit(1 << bitfields::i2c::RD_WRN);
            self.device.CR2.set_bit(1 << bitfields::i2c::START);
            self
        }

        pub unsafe fn get_rxdr(&self) -> u32 {
            self.device.RXDR.read_register()
            // self
        }

        pub unsafe fn get_isr(&self) -> u32 {
            self.device.ISR.read_register()
            // self
        }

        unsafe fn set_cr1_register(&self, value: u32) {
            self.device.CR1.set_bit(value);
        }

        pub unsafe fn set_cr2_register(&self, value: u32) {
            self.device.CR2.set_bit(value);
        }

        unsafe fn set_oar1_register(&self, value: u32) {
            self.device.OAR1.set_bit(value);
        }

        unsafe fn set_oar2_register(&self, value: u32) {
            self.device.OAR2.set_bit(value);
        }

        unsafe fn set_txdr_register(&self, value: u32) {
            // self.device.TIMINGR.set_bit(value);
            self.device.TXDR.set_bit(value);
        }

        unsafe fn set_timing2_register(&self, value: u32) {
            // self.device.TIMINGR.set_bit(value);
            self.device.TIMINGR.replace_whole_register(value);
        }

        unsafe fn set_timeoutr_register(&self, value: u32) {
            self.device.TIMEOUTR.set_bit(value);
        }

        pub unsafe fn set_isr_register(&self, value: u32) {
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

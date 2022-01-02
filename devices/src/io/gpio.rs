//!
//! This module provides basic access to the gpio registers.
//! The configuration of the ports can easily be done with
//! a provided builder.
//!
use super::super::bus::rcc;
use super::super::generic::platform::stm32f3x;
use super::super::generic::traits::primitive_extensions;
use super::super::registerblocks::gpio::GPIO;

pub mod gpio {
    //---------------------------------------------------------------//
    //----------------------------IMPORTS----------------------------//
    //---------------------------------------------------------------//
    use super::rcc;
    use super::stm32f3x::adresses;
    use super::stm32f3x::bitfields;
    use super::primitive_extensions::BitOps;
    use super::GPIO;
    use core::ptr;

    //---------------------------------------------------------------//
    //------------------------TYPE DEFINITONS------------------------//
    //---------------------------------------------------------------//
    pub enum OutputTypes {
        PushPull,
        OpenDrain,
    }

    pub enum ModerTypes {
        InputMode,
        GeneralPurposeOutputMode,
        AlternateFunctionMode,
        AnalogMode,
    }

    pub enum OutputState {
        High,
        Low,
    }

    //---------------------------------------------------------------//
    //-----------------------STRUCT-DEFINITONS-----------------------//
    //---------------------------------------------------------------//
    pub struct GpioDevice {
        port: &'static GPIO,
        pin: u32,
    }

    //---------------------------------------------------------------//
    //---------------------STRUCT-IMPLEMENTATIONS--------------------//
    //---------------------------------------------------------------//
    impl GpioDevice {
        ///
        /// Returns a GPIO Object which can be configured to different function modes.
        ///
        /// # Arguments
        ///
        /// * `port_mnemonic` - A string that describes the port name, e.g. "A" .
        /// * `pin_number` - An u8 variable to set the pin according to selected port .
        ///
        /// # Returns
        /// * `GpioPort Struct Object`
        ///
        pub unsafe fn new(port_mnemonic: &str, pin_number: u32) -> GpioDevice {
            let gpio_base = match port_mnemonic {
                "A" => {
                    rcc::rcc::activate_gpio_bus_clock(port_mnemonic);
                    adresses::gpio::GPIOA_BASE
                }
                "B" => {
                    rcc::rcc::activate_gpio_bus_clock(port_mnemonic);
                    adresses::gpio::GPIOB_BASE
                }
                "C" => {
                    rcc::rcc::activate_gpio_bus_clock(port_mnemonic);
                    adresses::gpio::GPIOC_BASE
                }
                "E" => {
                    rcc::rcc::activate_gpio_bus_clock(port_mnemonic);
                    adresses::gpio::GPIOE_BASE
                }
                _ => adresses::gpio::GPIOA_BASE,
            };

            GpioDevice {
                port: GPIO::new(gpio_base),
                pin: pin_number,
            }
        }

        pub fn as_output(self) -> GpioDevice {
            self.set_moder(ModerTypes::GeneralPurposeOutputMode);
            self
        }

        pub fn as_input(self) -> GpioDevice {
            self.set_moder(ModerTypes::InputMode);
            self
        }

        pub fn as_push_pull(self) -> GpioDevice {
            self.set_otyper(OutputTypes::PushPull);
            self
        }

        pub fn as_open_drain(self) -> GpioDevice {
            self.set_otyper(OutputTypes::OpenDrain);
            self
        }

        pub fn turn_on(&self) {
            self.set_odr(OutputState::High);
        }

        pub fn turn_off(&self) {
            self.set_odr(OutputState::High);
        }

        fn set_moder(&self, moder_type: ModerTypes) {
            match moder_type {
                ModerTypes::InputMode => {
                    self.port
                        .moder
                        .set_bit(bitfields::gpio::INPUT << self.pin * 2);
                }
                ModerTypes::GeneralPurposeOutputMode => {
                    self.port
                        .moder
                        .set_bit(bitfields::gpio::GENERALPURPOSEOUTPUT << self.pin * 2);
                }
                ModerTypes::AlternateFunctionMode => {
                    self.port
                        .moder
                        .set_bit(bitfields::gpio::ALTERNATE << self.pin * 2);
                }
                ModerTypes::AnalogMode => {
                    self.port
                        .moder
                        .set_bit(bitfields::gpio::ANALOG << self.pin * 2);
                }
            };
        }

        // 11.4.6 GPIO port output data register (GPIOx_ODR) (x = A..H)
        fn set_odr(&self, odr_type: OutputState) {
            match odr_type {
                OutputState::High => {
                    self.port.odr.set_bit((0b1 as u32) << self.pin);
                }
                OutputState::Low => {
                    self.port.odr.set_bit((0b1 as u32) << self.pin + 16);
                }
            };
        }
        // 11.4.2 GPIO port output type register
        fn set_otyper(&self, output_type: OutputTypes) {
            match output_type {
                OutputTypes::PushPull => {
                    self.port.otyper.clear_bit(0b1 << self.pin);
                }
                OutputTypes::OpenDrain => {
                    self.port.otyper.set_bit(0b1 << self.pin);
                }
            };
        }
        fn into_af(&self, af_number: u32) {
            let alternate_function_register = if self.pin < 8 {
                self.port.afrl
            } else {
                self.port.afrh
            };

            let mut pin = self.pin;
            if self.pin > 7 {
                pin -= 8;
            }
            unsafe {
                ptr::write_volatile(
                    alternate_function_register as *mut u32,
                    ptr::read_volatile(alternate_function_register as *const u32)
                        & !(0xF as u32) << pin * 4,
                );
                ptr::write_volatile(
                    alternate_function_register as *mut u32,
                    ptr::read_volatile(alternate_function_register as *const u32)
                        | af_number << pin * 4,
                );
            }
        }
    }
}

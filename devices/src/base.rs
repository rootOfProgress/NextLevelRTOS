//!
//! This module provides basic access to the gpio registers.
//! The configuration of the ports can easily be done with
//! a provided builder.
//!
use super::bus::rcc;
use super::generic::platform::stm32f3x;

pub mod gpio {
    //---------------------------------------------------------------//
    //----------------------------IMPORTS----------------------------//
    //---------------------------------------------------------------//
    use super::stm32f3x::adresses;
    use super::stm32f3x::bitfields;
    use super::stm32f3x::offsets;

    use super::rcc;
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
    pub struct GpioPort {
        gpio_base_adress: u32,
        pin_number: u8,
    }

    //---------------------------------------------------------------//
    //---------------------STRUCT-IMPLEMENTATIONS--------------------//
    //---------------------------------------------------------------//
    impl GpioPort {
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
        pub fn new(port_mnemonic: &str, pin_number: u8) -> GpioPort {
            GpioPort {
                gpio_base_adress: match port_mnemonic {
                    "A" => {
                        rcc::activate_gpio_bus_clock("A");
                        adresses::gpio::GPIOA_BASE
                    }
                    "E" => {
                        rcc::activate_gpio_bus_clock("E");
                        adresses::gpio::GPIOE_BASE
                    }
                    _ => adresses::gpio::GPIOA_BASE,
                },
                pin_number,
            }
        }

        pub fn as_output(self) -> GpioPort {
            self.set_moder(ModerTypes::GeneralPurposeOutputMode);
            self
        }

        pub fn as_input(self) -> GpioPort {
            self.set_moder(ModerTypes::InputMode);
            self
        }

        pub fn as_push_pull(self) -> GpioPort {
            self.set_otyper(OutputTypes::PushPull);
            self
        }

        pub fn as_open_drain(self) -> GpioPort {
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
            let mode_register = self.gpio_base_adress | offsets::gpio::GPIO_MODER;
            unsafe {
                match moder_type {
                    ModerTypes::InputMode => {
                        ptr::write_volatile(
                            mode_register as *mut u32,
                            ptr::read_volatile(mode_register as *const u32)
                                | bitfields::gpio::INPUT << (self.pin_number * 2),
                        );
                    }
                    ModerTypes::GeneralPurposeOutputMode => {
                        ptr::write_volatile(
                            mode_register as *mut u32,
                            ptr::read_volatile(mode_register as *const u32)
                                | (bitfields::gpio::GENERALPURPOSEOUTPUT << (self.pin_number * 2)),
                        );
                    }
                    ModerTypes::AlternateFunctionMode => {
                        ptr::write_volatile(
                            mode_register as *mut u32,
                            ptr::read_volatile(mode_register as *const u32)
                                | bitfields::gpio::ALTERNATE << (self.pin_number * 2),
                        );
                    }
                    ModerTypes::AnalogMode => {
                        ptr::write_volatile(
                            mode_register as *mut u32,
                            ptr::read_volatile(mode_register as *const u32)
                                | bitfields::gpio::ANALOG << (self.pin_number * 2),
                        );
                    }
                };
            }
        }

        // 11.4.6 GPIO port output data register (GPIOx_ODR) (x = A..H)
        fn set_odr(&self, odr_type: OutputState) {
            let output_data_register = self.gpio_base_adress | offsets::gpio::GPIO_ODR;
            unsafe {
                match odr_type {
                    OutputState::High => {
                        ptr::write_volatile(
                            output_data_register as *mut u32,
                            ptr::read_volatile(output_data_register as *const u32)
                                | (0b1 as u32) << self.pin_number,
                        );
                    }
                    OutputState::Low => {
                        ptr::write_volatile(
                            output_data_register as *mut u32,
                            ptr::read_volatile(output_data_register as *const u32)
                                | (0b1 as u32) << self.pin_number + 16,
                        );
                    }
                };
            }
        }
        // 11.4.2 GPIO port output type register
        fn set_otyper(&self, output_type: OutputTypes) {
            let output_type_register = self.gpio_base_adress | offsets::gpio::GPIO_OTYPER;
            unsafe {
                match output_type {
                    OutputTypes::PushPull => {
                        ptr::write_volatile(
                            output_type_register as *mut u32,
                            ptr::read_volatile(output_type_register as *const u32)
                                & (!(0b1 as u32) << self.pin_number),
                        );
                    }
                    OutputTypes::OpenDrain => {
                        ptr::write_volatile(
                            output_type_register as *mut u32,
                            ptr::read_volatile(output_type_register as *const u32)
                                | (0b1 as u32) << self.pin_number,
                        );
                    }
                };
            }
        }
        fn into_af(&self, af_number: u32) {
            let gpiox_af_offset = if self.pin_number < 8 {
                offsets::gpio::GPIO_AFRL
            } else {
                offsets::gpio::GPIO_AFRH
            };
            let alternate_function_register = self.gpio_base_adress | gpiox_af_offset;

            let mut pin = self.pin_number;
            if self.pin_number > 7 {
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

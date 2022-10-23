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
    use super::primitive_extensions::BitOps;
    use super::rcc;
    use super::stm32f3x::adresses;
    use super::stm32f3x::bitfields;
    use super::GPIO;

    //---------------------------------------------------------------//
    //------------------------TYPE DEFINITONS------------------------//
    //---------------------------------------------------------------//
    pub enum OutputTypes {
        PushPull,
        OpenDrain,
    }

    pub enum SpeedModes {
        Low,
        Medium,
        Fast,
        High,
    }

    pub enum PullTypes {
        Nothing,
        PullUp,
        PullDown,
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
                _ => panic!(),
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

        pub fn as_alternate_function(self) -> GpioDevice {
            self.set_moder(ModerTypes::AlternateFunctionMode);
            self
        }

        pub fn as_input(self) -> GpioDevice {
            self.set_moder(ModerTypes::InputMode);
            self
        }

        pub fn as_pull_up(self) -> GpioDevice {
            self.set_pupdr(PullTypes::PullUp);
            self
        }

        pub fn as_high_speed(self) -> GpioDevice {
            self.set_ospeedr(SpeedModes::High);
            self
        }

        pub fn as_open_drain(self) -> GpioDevice {
            self.set_otyper(OutputTypes::OpenDrain);
            self
        }

        pub fn as_push_pull(self) -> GpioDevice {
            self.set_otyper(OutputTypes::PushPull);
            self
        }

        pub fn is_pressed(&self) -> bool {
            let reg_content = self.port.idr.read_register();
            // let pin_masked = 0xFFFF & !(self.pin);
            if (reg_content & (1 << self.pin)) > 0 {
                return true;
            }
            false
        }

        fn set_ospeedr(&self, speed: SpeedModes) {
            // @todo: those are EXCLUSIVE!! clear field first!
            match speed {
                SpeedModes::Low => {
                    self.port.moder.set_bit(00 << self.pin * 2);
                }
                SpeedModes::Medium => {
                    self.port.moder.set_bit(01 << self.pin * 2);
                }
                SpeedModes::High => {
                    self.port.moder.set_bit(10 << self.pin * 2);
                }
                SpeedModes::Fast => {
                    self.port.moder.set_bit(0b11 << self.pin * 2);
                }
            };
        }

        pub fn turn_on(&self) {
            self.set_odr(OutputState::High);
        }

        pub fn turn_off(&self) {
            self.set_odr(OutputState::High);
        }

        pub fn as_af(self, af_number: u32) -> GpioDevice {
            self.into_af(af_number);
            self
        }

        fn set_moder(&self, moder_type: ModerTypes) {
            // @todo: those are EXCLUSIVE!! clear field first!
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

        fn set_pupdr(&self, pu_type: PullTypes) {
            match pu_type {
                _Nothing => {}
                PullTypes::PullUp => {
                    self.port.otyper.clear_bit(0b11 << 2 * self.pin);
                    self.port.otyper.set_bit(0b01 << 2 * self.pin);
                }
                PullTypes::PullDown => {
                    self.port.otyper.set_bit(0b10 << 2 * self.pin);
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
                    self.port.otyper.clear_bit(1 << self.pin);
                }
                OutputTypes::OpenDrain => {
                    self.port.otyper.set_bit(1 << self.pin);
                }
            };
        }
        fn into_af(&self, af_number: u32) {
            unsafe {
                if self.pin < 8 {
                    self.port.afrl.clear_bit((0xF as u32) << self.pin * 4);
                    self.port.afrl.set_bit(af_number << self.pin * 4);
                } else {
                    // asm!("bkpt");
                    let pin = self.pin - 8;
                    // asm!("bkpt");
                    let _toclear = (0xF as u32) << (pin * 4);
                    let _toset = af_number << (pin * 4);
                    // self.port.afrh.clear_bit((0xF as u32) << (pin * 4));
                    // asm!("bkpt");
                    self.port.afrh.set_bit(af_number << (pin * 4));
                    // asm!("bkpt");
                    // core::ptr::write(0x4800_0024 as *mut u32, 0x0400_0000);
                };
            }
        }
    }
}

use super::generic::platform::stm32f3x;

pub mod rcc {
    use super::stm32f3x::adresses;
    use super::stm32f3x::bitfields;
    use super::stm32f3x::offsets;

    use core::ptr;

    pub fn activate_gpio_bus_clock(port_mnemonic: &str) {
        unsafe {
            match port_mnemonic {
                "A" => ptr::write((adresses::RCC | offsets::rcc::RCC_AHBENR) as *mut u32, ptr::read((adresses::RCC | offsets::rcc::RCC_AHBENR) as *mut u32) | 0b1 << bitfields::rcc::IOPAEN),
                "E" => ptr::write((adresses::RCC | offsets::rcc::RCC_AHBENR) as *mut u32, ptr::read((adresses::RCC | offsets::rcc::RCC_AHBENR) as *mut u32) | 0b1 << bitfields::rcc::IOPEEN),
                _ => {}
            };
        }
    }
}

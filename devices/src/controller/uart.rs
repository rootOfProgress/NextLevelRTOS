use super::super::generic::cpu;
use super::super::generic::platform;

// connected to dma ch  4 (tx) / 5 (rx) 
pub mod uart {
    use super::cpu::system;
    use super::platform::stm32f3x::{adresses, offsets};

    const USART1_BASE: u32 = adresses::USART1_BASEADRESS;
    use core::ptr::{read_volatile, write_volatile};

    // use crate::generic::platform::{adresses, offsets};
    pub struct UsartX {
        usart_base_adress: u32,
        bus_number: u8,
        baudrate: u32,
    }
    pub fn new(bus_number: u8, baudrate: u32) -> UsartX {
        UsartX {
            usart_base_adress: match bus_number {
                1 => USART1_BASE,
                _ => USART1_BASE,
            },
            bus_number,
            baudrate,
        }
    }
    impl UsartX {
        pub fn enable(&self) {
            let usartx_brr = self.usart_base_adress | offsets::usart1::BRR;
            let baudrate_divisor = system::CLOCK / self.baudrate;
            // clk / 9600 baud
            unsafe {
                write_volatile(usartx_brr as *mut u32, baudrate_divisor);

                let usart1_cr1 = self.usart_base_adress;
                let existing_value = read_volatile(usart1_cr1 as *const u32);
                write_volatile(usart1_cr1 as *mut u32, existing_value | (0b1100));
                let existing_value = read_volatile(usart1_cr1 as *const u32);
                write_volatile(usart1_cr1 as *mut u32, existing_value | (0b1));
            }
        }
    }

    // propably the world's worst and slowest function to print stupid integers on
    // a screen
    pub fn print_dec(mut dec: u32) {
        let usart2_tdr = adresses::USART1_BASEADRESS | offsets::usart1::TDR;
        let usart2_isr = adresses::USART1_BASEADRESS | offsets::usart1::ISR;
        let mut buffer: [u8; 32] = unsafe { core::mem::zeroed() };
        let mut cnt: u8 = 0;
        while dec > 0 {
            buffer[cnt as usize] = (dec % 10 + 0x30) as u8;
            dec /= 10;
            cnt += 1;
        }
        for c in IntoIterator::into_iter(buffer).rev() {
            unsafe {
                write_volatile(usart2_tdr as *mut u32, c as u32);
                while !((read_volatile(usart2_isr as *mut u32) & 0x80) != 0) {}
            }
        }
    }

    pub fn print_from_ptr(mut ptr_start: *mut u8) {
        let usart2_tdr = adresses::USART1_BASEADRESS | offsets::usart1::TDR;
        let usart2_isr = adresses::USART1_BASEADRESS | offsets::usart1::ISR;

        unsafe {
            while *ptr_start != b'\0' {
                write_volatile(usart2_tdr as *mut u32, *ptr_start as u32);
                ptr_start = ptr_start.add(1);
                while !((read_volatile(usart2_isr as *const u32) & 0x80) != 0) {}
            }
        }
    }

    pub fn print_str(msg: &str) {
        let usart2_tdr = adresses::USART1_BASEADRESS | offsets::usart1::TDR;
        let usart2_isr = adresses::USART1_BASEADRESS | offsets::usart1::ISR;

        for c in msg.chars() {
            unsafe {
                write_volatile(usart2_tdr as *mut u32, c as u32);
                while !((read_volatile(usart2_isr as *const u32) & 0x80) != 0) {}
            }
        }
    }
}

use super::super::bus::rcc;
use super::super::generic::platform::stm32f3x;
use super::super::generic::platform::stm32f3x::bitfields;
use super::super::generic::traits::primitive_extensions;
use super::super::io::i2c::i2c::{get_i2c_dev, I2cDevice};
use super::super::registerblocks::usart::USART;

static mut column: usize = 0;
static mut row: usize = 0;
static mut buffer: [[char; 3]; 4] = [
    ['0', '0', '0'],
    ['0', '0', '0'],
    ['0', '0', '0'],
    ['0', '0', '0'],
];
pub static mut res: [u32; 4] = [0, 0, 0, 0];

// extern res: [u32; 4] = [0,0,0,0];
// stm32f4
// const USART1_DR: u32 = 0x4001_1004;
// const USART1_SR: u32 = 0x4001_1000;

// stm32f3
const USART1_RDR: u32 = stm32f3x::adresses::USART1_BASEADRESS | 0x24;
const USART1_ISR: u32 = stm32f3x::adresses::USART1_BASEADRESS | 0x1C;
const USART1_TDR: u32 = stm32f3x::adresses::USART1_BASEADRESS | 0x28;

const CRLF: &str = "\r\n";

pub trait iostream {
    fn print(&self);
    fn println(&self);
}

///
/// Appends print methods to &str primitive. The println method
/// sends a newline character and a carriage return after the
/// payload has been succesful transmitted.
///
impl iostream for &str {
    fn print(&self) {
        for c in self.chars() {
            transmit(c as u32);
        }
    }
    fn println(&self) {
        for c in self.chars() {
            transmit(c as u32);
        }
        for c in CRLF.chars() {
            transmit(c as u32);
        }
    }
}

pub mod usart {
    use super::primitive_extensions::BitOps;
    use super::rcc;
    use super::stm32f3x::{adresses, bitfields};
    use super::USART;
    pub struct UsartDevice {
        device: &'static USART,
        baudrate: u32,
    }

    impl UsartDevice {
        pub unsafe fn new(baudrate: u32) -> UsartDevice {
            rcc::rcc::activate_usart1_bus_clock();
            UsartDevice {
                device: USART::new(adresses::USART1_BASEADRESS),
                baudrate,
            }
        }

        pub fn dma_tx(self) -> UsartDevice {
            self.enable_dma_tx();
            self
        }

        pub fn enable(&self) -> &UsartDevice {
            self.setup_usart_controller();
            self
        }

        fn enable_dma_tx(&self) {
            self.device.cr3.set_bit(0b1 << 7);
        }

        fn setup_usart_controller(&self) {
            // STMF4 SPECIFIC DO NOT DELETE
            //// bus is feed from HSI CLK with 16 MHZ.
            //// how to set brr register:
            //// page 981 table 136 entry 2:
            //// baud: 104.1875 -> 104 | 16 * 0.1875 : 0d1667
            //self.device.brr.set_bit(1667);
            //self.device.cr1.set_bit(0b1100);
            //self.device.cr1.set_bit(0b1 << 13);

            // 8mhz / 9600 bps
            let baudrate_divisor = 8_000_000 / 9600;
            self.device.brr.replace_whole_register(baudrate_divisor);
            self.device.cr1.set_bit(
                1 << bitfields::usart1::TE
                    | 1 << bitfields::usart1::RE
                    | 1 << bitfields::usart1::RXNEIE,
            );

            self.device.cr1.set_bit(1 << bitfields::usart1::UE);

            // enable cpu usart ir
            unsafe {
                core::ptr::write_volatile(0xE000E104 as *mut u32, 1 << 5);
            }
        }
    }
}

///
/// Writes one byte into transmit register und polls interrupt flag until transmission in complete. Acts as
/// as "blocking send".
///
/// # Arguments
///
/// * `c` - A bytee representing the char to be sent.
///
/// # Returns
/// * `Nothing`
///
fn transmit(c: u32) {
    unsafe {
        // currently not clear if buffer needs to be flushed first
        //core::ptr::write_volatile(USART1_DR as *mut u32, core::ptr::read_volatile(USART1_DR as *const u32) & !(0x1FF));
        // core::ptr::write_volatile(
        //     USART1_DR as *mut u32,
        //     core::ptr::read_volatile(USART1_DR as *const u32) | c,
        // );
        core::ptr::write_volatile(USART1_TDR as *mut u32, c);
        while !((core::ptr::read_volatile(USART1_ISR as *const u32) & 0x80) != 0) {}
    }
}

fn process() {
    let mut result_idx = 0;
    unsafe {
        for r in 0..4 {
            let mut sum: u32 = 0;
            let mut n: u32 = 100;
            for c in 0..3 {
                let num = buffer[r][c] as u8;
                sum += (((buffer[r][c] as u8 - 0x30) as u8) as u32 * n) as u32;
                n /= 10;
            }
            res[result_idx] = sum;
            result_idx += 1;
        }
    }
}

#[no_mangle]
pub extern "C" fn Usart1_MainISR() {
    unsafe {
        // dummy leave it
        if (1 == 2) {
            let mut rx_data: u8 = core::ptr::read_volatile(USART1_RDR as *const u32) as u8;
            transmit(rx_data as u32);
            // asm!("bkpt");

            if (rx_data != 10) {
                if ((rx_data as char) == '|') {
                    row += 1;
                    column = 0;
                } else if ((rx_data as char) == ';') {
                    row = 0;
                    column = 0;
                    process();
                } else {
                    buffer[row][column] = rx_data as char;
                    column += 1;
                }
            }
        }
        let mut rx_data: u8 = core::ptr::read_volatile(USART1_RDR as *const u32) as u8;
        transmit(rx_data as u32);

        let d = get_i2c_dev();
        d.set_cr2_register(0x53 << 1 | 1 << 16 | 1 << 25);
        d.start();
        // while !((d.get_isr() & bitfields::i2c::TXE) != 0) ){};
        d.write(0x66);
       // d.set_isr_register(1 << bitfields::i2c::TXE);
        //while !(((d.get_isr() & bitfields::i2c::ADDR) != 0)) {};
        asm!("bkpt");
        // i2c.start();
        // i2c.write(0xFF);

        // loop {
        // }
    }
    // context_switch();
}

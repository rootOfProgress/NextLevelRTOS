use super::super::bus::rcc;
use super::super::generic::platform::stm32f3x;

use super::super::generic::traits::primitive_extensions;
use super::super::io::i2c::i2c::get_i2c_dev;
use super::super::registerblocks::usart::USART;
use super::super::sys::tick::init_systick;
// use super::super::ext::adx345;
use core::intrinsics::{volatile_load, volatile_store};

static mut column: usize = 0;
static mut row: usize = 0;
static mut buffer: [[char; 3]; 4] = [
    ['0', '0', '0'],
    ['0', '0', '0'],
    ['0', '0', '0'],
    ['0', '0', '0'],
];

// static mut byte_order: [usize; 4] = [1,0,3,2];
static mut i2c_buffer: [char; 16] = [
    '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',
];

static mut task_buffer: [u8; 512] = [
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
];

// static mut byte_low: u32 = 0;
// static mut byte_high: u32 = 0;

static mut i2c_idx: u32 = 0;
static mut i2c_payload: u32 = 0;
static mut sum: u32 = 0;
static mut task_target: u32 = 0x20001000;
static mut task_byte: u32 = 0;
static mut task_word: u32 = 0;

#[derive(PartialEq)]
enum RCV_STATE {
    PWM,
    I2C,
    TASK,
}

static mut rcv_state: RCV_STATE = RCV_STATE::TASK;

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

fn transmit_int(mut int_number: u32) {
    unsafe {
        volatile_store(
            0x4800_0014 as *mut u32,
            volatile_load(0x4800_0014 as *const u32) & !(1 << 1),
        );
        let mut cnt: u8 = 0;
        let mut buf: [u8; 32] = [
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0,
        ];
        while int_number > 0 {
            buf[cnt as usize] = (int_number % 10 + 0x30) as u8;
            int_number /= 10;
            cnt += 1;
        }
        for c in (0..cnt).rev() {
            core::ptr::write_volatile(USART1_TDR as *mut u32, buf[c as usize] as u32);
            while !((core::ptr::read_volatile(USART1_ISR as *const u32) & 0x80) != 0) {}
        }

        volatile_store(
            0x4800_0014 as *mut u32,
            volatile_load(0x4800_0014 as *const u32) & !1,
        );
        // volatile_store(
        //     0x4800_0014 as *mut u32,
        //     volatile_load(0x4800_0014 as *const u32) | 1 << 1,
        // );

        // // volatile_store(address as *mut u32, volatile_load(address) & !(bit_number));
        // volatile_store(USART1_TDR as *mut u32, (int_number & !(0xFFFF_FF00)) as u32);
        // while !((volatile_load(USART1_ISR as *const u32) & 0x80) != 0) {}
        // volatile_store(
        //     USART1_TDR as *mut u32,
        //     ((int_number & !(0xFFFF_00FF)) >> 8) as u32,
        // );
        // while !((volatile_load(USART1_ISR as *const u32) & 0x80) != 0) {}
        // volatile_store(
        //     USART1_TDR as *mut u32,
        //     ((int_number & !(0xFF00_FFFF)) >> 16) as u32,
        // );
        // while !((volatile_load(USART1_ISR as *const u32) & 0x80) != 0) {}
        // volatile_store(USART1_TDR as *mut u32, (int_number >> 24) as u32);
        // while !((volatile_load(USART1_ISR as *const u32) & 0x80) != 0) {}
        // volatile_store(
        //     0x4800_0014 as *mut u32,
        //     volatile_load(0x4800_0014 as *const u32) & !1,
        // );

        // volatile_store(
        //     0x4800_0014 as *mut u32,
        //     volatile_load(0x4800_0014 as *const u32) | 1,
        // );
    }
}

fn process() {
    let mut result_idx = 0;
    unsafe {
        for r in 0..4 {
            let mut sum_pwm: u32 = 0;
            let mut n: u32 = 100;
            for c in 0..3 {
                let _num = buffer[r][c] as u8;
                sum_pwm += (((buffer[r][c] as u8 - 0x30) as u8) as u32 * n) as u32;
                n /= 10;
            }
            res[result_idx] = sum_pwm;
            result_idx += 1;
        }
    }
}

#[no_mangle]
pub extern "C" fn Usart1_MainISR() {
    unsafe {
        let rx_data: u8 = core::ptr::read_volatile(USART1_RDR as *const u32) as u8;
        // echo
        // transmit(rx_data as u32);

        if rx_data as char == 'I' {
            rcv_state = RCV_STATE::I2C;
            return;
        } else if rx_data as char == 'P' {
            rcv_state = RCV_STATE::PWM;
            return;
        } else if rx_data as char == 'T' {
            rcv_state = RCV_STATE::TASK;
            return;
        } else if rx_data as char == 'f' {
            task_target = 0x20001000;
            // flush memory
            for i in (0x0..0x60).step_by(0x4) {
                volatile_store((task_target + i) as *mut u32, 0x0);
            }
            task_target = 0x20001000;
            task_byte = 0;
            task_word = 0;
            return;
        } /* else if rx_data as char == 'S' {
            core::ptr::write_volatile(
                0xE000_E010 as *mut u32,
                core::ptr::read_volatile(0xE000_E010 as *const u32) | 0b1,
            );
            return;
        }
 */
        // dummy leave it
        if rcv_state == RCV_STATE::TASK {
            task_word |= (rx_data as u32) << task_byte * 8;
            if task_byte == 3 {
                volatile_store(task_target as *mut u32, task_word);
                task_target += 0x4;
                task_byte = 0;
                task_word = 0;
            } else {
                task_byte += 1;
            }
        } else if rcv_state == RCV_STATE::PWM {
            if rx_data != 10 {
                if (rx_data as char) == '|' {
                    row += 1;
                    column = 0;
                } else if (rx_data as char) == ';' {
                    row = 0;
                    column = 0;
                    process();
                } else {
                    buffer[row][column] = rx_data as char;
                    column += 1;
                }
            }
        } else if rcv_state == RCV_STATE::I2C {
            if rx_data as char == ';' {
                if i2c_buffer[0] as char == 'w' {
                    let _register = sum >> 8;
                    let mut num_bytes = 1;
                    let content = sum & !(0xFF << 8);
                    if content != 0 {
                        num_bytes = 2;
                    }

                    let d = get_i2c_dev();

                    d.write(sum, num_bytes, 0x53);
                    sum = 0;
                } else if i2c_buffer[0] as char == 'p' {
                    // p = payload
                    let n = 1;
                    let mut multi = 1;

                    // undoes #A
                    i2c_idx -= 1;
                    while i2c_idx >= n {
                        sum += ((i2c_buffer[i2c_idx as usize] as u8 - 0x30) as u32) * multi;
                        i2c_idx -= 1;
                        multi *= 10;
                    }
                } else if i2c_buffer[0] as char == 'r' {
                    let d = get_i2c_dev();
                    d.read(sum);
                    let result = d.get_rxdr();
                    if (result == 0) {
                        "0".println();
                    } else {
                        transmit_int(result);
                        "".println();
                    }
                    sum = 0;
                } else if i2c_buffer[0] as char == 't' {
                    let d = get_i2c_dev();
                    // d.rw_test();
                    // transmit_int(123429);
                }
                i2c_idx = 0;
            } else if rx_data as char != '\n' {
                i2c_buffer[i2c_idx as usize] = rx_data as char;
                // #A
                i2c_idx += 1;
            }
        }
    }
}

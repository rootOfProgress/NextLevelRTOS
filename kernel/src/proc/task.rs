#![no_std]
extern crate cpu;
use super::tcb::TCB;
use crate::mem::malloc::{allocate, MemoryResult};
use cpu::core::CoreRegister;
use core::cell::Cell;

pub mod task {
    use super::CoreRegister;
    use super::Cell;
    use super::{allocate, MemoryResult};

    pub fn create_task(target: u32, end_destination: u32) -> Frame {
        let process_frame = Frame::new();
        let register =
            (process_frame.buffer.get() - core::mem::size_of::<CoreRegister>() as u32) as *mut CoreRegister;
        unsafe {
            (*register).r0 = 0xAB;
            (*register).psr = 0x21000000;
            (*register).r4 = 0x66;
            (*register).r11 = 0xBC;
            (*register).pc = target;
            (*register).lr = end_destination;
        }
        process_frame
    }

    #[repr(C)]
    pub struct Frame {
        buffer: Cell<u32>,
    }

    impl Frame {
        pub fn new() -> Self {
            unsafe {
                let process_memory: Option<MemoryResult> =
                    allocate(512 + core::mem::size_of::<CoreRegister>() as u32);

                match process_memory {
                    Some(result) => Frame {
                        buffer: Cell::new(result.end_address),
                    },
                    None => {
                        loop{}
                        todo!()
                    }
                }
            }
        }

        pub fn set_target_addr(&mut self, target: u32) {
            let register = (self.buffer.get() - core::mem::size_of::<CoreRegister>() as u32)
                as *mut CoreRegister;
            unsafe {
                (*register).pc = target;
                (*register).psr = 0x21000000;
            }
        }

        pub fn get_r4_location(&self) -> u32 {
            let register = (self.buffer.get() - core::mem::size_of::<CoreRegister>() as u32)
                as *mut CoreRegister;
            unsafe { core::ptr::addr_of!((*register).r4) as u32 }
        }
    }
}

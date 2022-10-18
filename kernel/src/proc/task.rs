#![no_std]
extern crate cpu;
use crate::mem::malloc::{memory_mng_allocate_process};
use cpu::core::CoreRegister;
use super::tcb::TCB;

pub mod task {
    use super::TCB;
    use super::CoreRegister;
    use super::memory_mng_allocate_process;

    // #[repr(C)]
    // pub struct TCB<'a> {
    //     // Current stack pointer value
    //     pub pid: u32,
    //     pub name: &'a str,
    //     pub sp: u32,
    // }

    pub fn create_task(target: u32, end_destination: u32) -> Option<Frame> {
        let mut frame = Frame::new();

        frame = match frame {
            Some(mut p) => {
                let register = p.buffer as *mut CoreRegister;
                unsafe {
                    (*register).pc = target;
                    (*register).lr = end_destination;
                    (*register).psr = 0x21000000;
                }
                // p.set_target_addr(target);
                // p.set_end_destination_addr(end_destination);
                Some(p)
            }
            None => None,
        };
        frame
    }

    #[repr(C)]
    pub struct Frame {
        buffer: *mut u32
    }

    impl Frame {
        pub fn new() -> Option<Self> {
            let process_memory = memory_mng_allocate_process(32 + core::mem::size_of::<CoreRegister>() as u32);
            Some(Frame {
                buffer: process_memory as *mut u32
            })
        }

        pub fn set_target_addr(&mut self, target: u32) {
            let register = self.buffer as *mut CoreRegister;
            unsafe {
                (*register).pc = target;
                (*register).psr = 0x21000000;
            }
        }
    
        pub fn get_r4_location(&self) -> u32 {
            let register = self.buffer as *mut CoreRegister;
            unsafe {
                core::ptr::addr_of!((*register).r4) as u32
            }
        }
    }
}

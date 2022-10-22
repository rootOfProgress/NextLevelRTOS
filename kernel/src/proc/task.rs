#![no_std]
extern crate cpu;
use super::tcb::TCB;
use crate::mem::malloc::{allocate, MemoryResult};
use cpu::core::CoreRegister;

pub mod task {
    use super::CoreRegister;
    use super::TCB;
    use super::{allocate, MemoryResult};

    // #[repr(C)]
    // pub struct TCB<'a> {
    //     // Current stack pointer value
    //     pub pid: u32,
    //     pub name: &'a str,
    //     pub sp: u32,
    // }

    pub fn create_task(target: u32, end_destination: u32) -> Frame {
        let mut p = Frame::new();
        let register =
            (p.buffer as u32 - core::mem::size_of::<CoreRegister>() as u32) as *mut CoreRegister;
        unsafe {
            (*register).r0 = 0xAB;
            (*register).psr = 0x21000000;
            (*register).r4 = 0x66;
            (*register).r11 = 0xBC;
            (*register).pc = target;
            (*register).lr = end_destination;
        }
        // frame = match frame {
        //     Some(mut p) => {
        //         let register = (p.buffer as u32 - core::mem::size_of::<CoreRegister>() as u32) as *mut CoreRegister;
        //         unsafe {
        //             (*register).r0 = 0xAB;
        //             (*register).psr = 0x21000000;
        //             (*register).r4 = 0x66;
        //             (*register).r11 = 0xBC;
        //             (*register).pc = target;
        //             (*register).lr = end_destination;
        //         }
        //         // p.set_target_addr(target);
        //         // p.set_end_destination_addr(end_destination);
        //         Some(p)
        //     }
        //     None => None,
        // };
        p
    }

    #[repr(C)]
    pub struct Frame {
        buffer: *mut u32,
    }

    impl Frame {
        pub fn new() -> Self {
            unsafe {
                let process_memory: Option<MemoryResult> =
                    allocate(32 + core::mem::size_of::<CoreRegister>() as u32);

                match process_memory {
                    Some(result) => Frame {
                        buffer: result.end_address as *mut u32,
                    },
                    None => {
                        todo!()
                    }
                }
            }
        }

        pub fn set_target_addr(&mut self, target: u32) {
            let register = (self.buffer as u32 - core::mem::size_of::<CoreRegister>() as u32)
                as *mut CoreRegister;
            unsafe {
                (*register).pc = target;
                (*register).psr = 0x21000000;
            }
        }

        pub fn get_r4_location(&self) -> u32 {
            let register = (self.buffer as u32 - core::mem::size_of::<CoreRegister>() as u32)
                as *mut CoreRegister;
            unsafe { core::ptr::addr_of!((*register).r4) as u32 }
        }
    }
}

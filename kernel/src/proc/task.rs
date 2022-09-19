#![no_std]
extern crate cpu;

pub mod task {
    use super::cpu;

    #[repr(C)]
    pub struct TCB<'a> {
        // Current stack pointer value
        pub pid: u32,
        pub name: &'a str,
        pub sp: u32,
    }

    #[repr(C)]
    pub struct Frame {
        buffer: [u32; 256],
        initialized_core_registers: cpu::core::CoreRegister,
    }

    pub fn create_task(target: u32, end_destination: u32) -> Option<Frame> {
        let cpu_register_set = cpu::core::CoreRegister::default();
        let mut frame = Frame::new(cpu_register_set, 256);
        frame = match frame {
            Some(mut p) => {
                p.set_target_addr(target);
                p.set_end_destination_addr(end_destination);
                Some(p)
            }
            None => None,
        };
        frame
    }

    impl Frame {
        pub fn new(core: cpu::core::CoreRegister, buffer_size: u32) -> Option<Self> {
            let dynamic_buffer = core::alloc::Layout::from_size_align(buffer_size as usize, 4);
            match dynamic_buffer {
                Ok(buffer) => Some(Frame {
                    buffer: unsafe { core::mem::zeroed() },
                    initialized_core_registers: core,
                }),
                Err(_) => {
                    // todo: print kernel panic over serial later on
                    None
                }
            }
        }

        pub fn get_frame_size(&mut self) -> u32 {
            core::mem::size_of::<cpu::core::CoreRegister>() as u32
        }

        pub fn set_target_addr(&mut self, target: u32) {
            self.initialized_core_registers.pc = target;
            // just for testing
            // self.initialized_core_registers.lr = target;
            self.initialized_core_registers.psr = 0x21000000;
        }
        pub fn get_target_addr(&mut self) -> u32 {
            self.initialized_core_registers.pc
        }

        pub fn set_end_destination_addr(&mut self, destination: u32) {
            self.initialized_core_registers.lr = destination;
        }

        pub fn get_r4_location(&self) -> u32 {
            core::ptr::addr_of!(self.initialized_core_registers.r4) as u32
        }
    }
}

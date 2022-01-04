extern crate cpu;
#[repr(C)]
pub struct Frame {
    buffer: core::alloc::Layout,
    initialized_core_registers: cpu::core::CoreRegister,
}

impl Frame {
    pub fn new(core: cpu::core::CoreRegister, buffer_size: u32) -> Option<Self> {
        let dynamic_buffer = core::alloc::Layout::from_size_align(buffer_size as usize, 4);
        match dynamic_buffer {
            Ok(buffer) => Some(Frame {
                buffer,
                initialized_core_registers: core,
            }),
            Err(_) => {
                // todo: print kernel panic over serial later on
                None
            }
        }
    }
    pub fn set_target_addr(&mut self, target: u32) {
        self.initialized_core_registers.pc = target;
    }
    pub fn get_r4_location(&self) -> u32 {
        core::ptr::addr_of!(self.initialized_core_registers.r4) as u32
    }
}

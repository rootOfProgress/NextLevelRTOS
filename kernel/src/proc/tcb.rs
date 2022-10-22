///
/// Task Control Block, 32 Bit aligned.
///
#[repr(C)]
pub struct TCB {
    pub pid: u32,
    pub sp: u32,
    pub memory_lower_bound: u32,
    pub memory_upper_bound: u32,
}

impl TCB {
    pub fn create(
        stack_pointer: u32,
        pid: u32,
        memory_lower_bound: u32,
        memory_upper_bound: u32,
    ) -> TCB {
        TCB {
            pid,
            sp: stack_pointer,
            memory_lower_bound,
            memory_upper_bound,
        }
    }
}

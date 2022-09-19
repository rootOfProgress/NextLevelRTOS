///
/// Task Control Block, 32 Bit aligned.
///
#[repr(C)]
pub struct TCB<'a> {
    // Current stack pointer value
    pub pid: u32,
    pub name: &'a str,
    pub sp: u32,
}

impl TCB<'_> {
    pub fn create(stack_pointer: u32, pid: u32, name: &str) -> TCB {
        TCB {
            pid,
            name,
            sp: stack_pointer,
        }
    }
}

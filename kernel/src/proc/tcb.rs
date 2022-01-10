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

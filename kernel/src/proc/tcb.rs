use core::sync::atomic::{AtomicU32, Ordering};

///
/// Task Control Block, 32 Bit aligned.
///
#[repr(C)]
#[repr(align(4))]
pub struct TCB {
    // Current stack pointer value
    sp: u32,
    //Task condition according to TaskStates
    state: TaskStates,
    // Task number
    pid: u32,
    memory_consumption: u32,
    runtime: u32,
}

///
/// Tasklist is fix located at 0x2000_0200.
/// TODO: Reserve this area in linker script somehow.
///
const TCB_START: u32 = 0x2000_0200;
const TCB_SIZE: u32 = core::mem::size_of::<TCB>() as u32;

static HEAP_SIZE: AtomicU32 = AtomicU32::new(0);
pub static CURRENTLY_SLEEPING: AtomicU32 = AtomicU32::new(0);
pub static CURRENT_TASK: AtomicU32 = AtomicU32::new(0);

/// Represents the possible states of a task.
pub enum TaskStates {
    READY,
    RUNNING,
    BLOCKED,
    TERMINATED,
}

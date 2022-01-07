use core::alloc::Layout;
use core::sync::atomic::{AtomicU32, Ordering};

///
/// Task Control Block, 32 Bit aligned.
///
#[repr(C)]
#[repr(align(4))]
pub struct TASKMETA {
    pub current_task: u8,
    pub number_of_tasks: u8,
}

///
/// Task Control Block, 32 Bit aligned.
///
#[repr(C)]
// #[repr(align(4))]
pub struct TCB {
    // Current stack pointer value
    pub sp: u32,
    //Task condition according to TaskStates
    // pub state: TaskStates,
    // Task number
    // pub pid: u32,
    // pub start_adress: u32,
    // pub runtime: u32,
}

///
/// Tasklist is fix located at 0x2000_0004.
///
///
pub const TASKMETA: u32 = 0x2000_0000;
pub const TCB_META: u32 = 0x2000_0000;
pub const TCB_PTR: u32 = 0x2000_0004;
pub const TCB_BLK: u32 = 0x2000_0024;
pub const TCB_SIZE: u32 = core::mem::size_of::<TCB>() as u32;

pub static CURRENTLY_SLEEPING: AtomicU32 = AtomicU32::new(0);
pub static CURRENT_TASK: AtomicU32 = AtomicU32::new(0);

/// Represents the possible states of a task.
pub enum TaskStates {
    READY,
    RUNNING,
    BLOCKED,
    TERMINATED,
}

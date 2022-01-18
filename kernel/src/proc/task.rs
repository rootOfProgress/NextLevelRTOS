use super::tcb::TCB;

pub fn create(stack_pointer: u32, pid: u32) -> TCB {
    TCB {
        pid,
        sp: stack_pointer,
    }
}

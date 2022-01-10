use super::tcb::TCB;

pub fn create(stack_pointer: u32, pid: u32, name: &str) -> TCB {
    TCB {
        pid,
        name,
        sp: stack_pointer,
    }
}

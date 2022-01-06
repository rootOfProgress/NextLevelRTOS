use super::tcb::{TaskStates, TCB, TCB_BLK, TCB_PTR, TCB_SIZE, TCB_META};

pub fn insert(proc_size: u32, stack_pointer: u32, pid: u32) -> u32 {
    let tcb_location = (pid * TCB_SIZE) + TCB_BLK;
    let tcb = unsafe { &mut *(tcb_location as *mut Option<TCB>) };

    *tcb = Some(TCB {
        sp: stack_pointer,
        state: TaskStates::READY,
        start_adress: stack_pointer - proc_size,
        pid,
        runtime: 0,
    });
    // let foo = core::ptr::addr_of!(tcb) as *const u32;
    unsafe {
        core::ptr::write_volatile((TCB_META) as *mut u32, *(core::ptr::addr_of!(tcb) as *const u32));
        core::ptr::write_volatile((TCB_PTR + pid) as *mut u32, *(core::ptr::addr_of!(tcb) as *const u32));
    }

    pid + 1
}

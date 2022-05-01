//---------------------------------------------------------------//
//----------------------------IMPORTS----------------------------//
//---------------------------------------------------------------//
use super::super::data::list::List;
use super::super::mem;
use super::task;
use devices::controller::uart::iostream;
static mut RUNNABLE_TASKS: u32 = 0;
static mut SLEEPING_TASKS: u32 = 0;

#[repr(C)]
struct bar {
    r0: u32,
    r1: u32,
}

extern "C" {
    ///
    /// Points to extern asm instructions.
    ///
    fn __get_r0_r1() -> bar;
    fn __br_to_init();
    // fn __trap(x: u32);
    fn __trap(arg: u32, id: u32);
    fn __load_process_context(stack_addr: u32);
    fn __save_process_context() -> u32;
}

pub fn init() {
    unsafe { RUNNABLE_TASKS = List::new() };
    unsafe { SLEEPING_TASKS = List::new() };
}

pub fn destroy() {
    unsafe {
        core::intrinsics::volatile_store(
            0xE000_E010 as *mut u32,
            core::ptr::read_volatile(0xE000_E010 as *const u32) & !0b1,
        );
        let list = &mut *(RUNNABLE_TASKS as *mut List);

        // lost forever right now
        list.delete_head_node();

        if list.get_size() == 0 {
            // wake up idle task
            // or: set cpu to sleep
        }

        let p = list.cursor_sp();

        core::intrinsics::volatile_store(
            0xE000_E010 as *mut u32,
            core::ptr::read_volatile(0xE000_E010 as *const u32) | 0b1,
        );
        // __trap(p, 0);
        __trap(p, 0);
    }
}

// experimental
pub fn sleep() {
    // let list_active = &mut *(RUNNABLE_TASKS as *mut List);
    // let list_sleeping = &mut *(SLEEPING_TASKS as *mut List);
    // if list.size > 1 {
    // }
}

pub fn shift_task() -> u32 {
    let list = unsafe { &mut *(RUNNABLE_TASKS as *mut List) };
    list.get_next_sp()
}

pub fn start_init_process() {
    unsafe {
        let stackpointer_value = shift_task();

        // start systick timer
        core::ptr::write_volatile(
            0xE000_E010 as *mut u32,
            core::ptr::read_volatile(0xE000_E010 as *const u32) | 0b1,
        );
        __trap( stackpointer_value, 0);
    }
}


pub fn spawn_task(function: u32, name: &str, buffer: u32) {
    unsafe {
        let list = &mut *(RUNNABLE_TASKS as *mut List);

        let task_address_space =
            mem::malloc::get_mem(core::mem::size_of::<cpu::core::CoreRegister>() as u32 + buffer);
        
        // alloc failed
        if task_address_space == 0 {
            let m = 3;
            return;
        }
        let cr = &mut *(((task_address_space + buffer)
            - core::mem::size_of::<cpu::core::CoreRegister>() as u32)
            as *mut cpu::core::CoreRegister);
        cr.r4 = 0x123;
        cr.pc = function;
        cr.lr = destroy as *const () as u32;
        cr.psr = 0x21000000;

        let pid = list.get_size();

        let tcb = task::create(core::ptr::addr_of!(cr.r4) as u32, pid, name);
        list.insert(tcb);
    }
}

pub fn yield_task() {
    context_switch();
}

pub fn context_switch() {
    unsafe {
        let old_sp = __save_process_context();
        let list = &mut *(RUNNABLE_TASKS as *mut List);
        list.update_tcb(old_sp);
        let sp = list.get_next_sp();
        let size = list.get_size();
        __load_process_context(sp);
    }
}

pub fn enable_systick() {
    unsafe {

        __trap(0,1);
    }
}

#[no_mangle]
pub extern "C" fn SysTick() {
   context_switch();
}

#[no_mangle]
pub extern "C" fn SVCall(arg: u32, id: u32) {
    unsafe {
        
    match id {
        0 => {
            __br_to_init();
        },
        1 => {
            let foo = 123;
            devices::sys::tick::init_systick(1280);
            devices::sys::tick::enable_systick();
        }
        _ => {
            let foo = 123;
        }
    }
    //  __br_to_init()
     };
}

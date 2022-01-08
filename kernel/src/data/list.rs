use super::super::proc::tcb::TCB;
use crate::mem::malloc::get_mem;
#[repr(C)]
struct Node {
    next: *const u32,
    data: TCB,
}

#[repr(C)]
pub struct List {
    size: u32,
    cursor: *const u32,
    head: *const u32,
    tail: *const u32,
}

impl List {
    pub fn new() -> u32 {
        let mem_start = get_mem(core::mem::size_of::<List>() as u32);
        let list = unsafe { &mut *(mem_start as *mut List) };
        *list = List {
            size: 0,
            cursor: core::ptr::null(),
            head: core::ptr::null(),
            tail: core::ptr::null(),
        };
        mem_start
    }
    pub fn insert(&mut self, tcb: TCB) {
        let mem_for_new_node = get_mem(core::mem::size_of::<Node>() as u32);
        let node = unsafe { &mut *(mem_for_new_node as *mut Node) };

        *node = Node {
            data: tcb,
            next: core::ptr::null(),
        };

        if self.head == core::ptr::null() {
            node.next = mem_for_new_node as *const u32;
            self.tail = mem_for_new_node as *const u32;
            self.head = mem_for_new_node as *const u32;
            self.cursor = mem_for_new_node as *const u32;
        } else {
            // append to tail / fifo
            node.next = self.head;
            let tail_node = unsafe { &mut *(self.tail as *mut Node) };
            tail_node.next = mem_for_new_node as *const u32;
            self.tail = mem_for_new_node as *const u32;
        }
        self.size += 1;
    }
    // returns sp only and shifts pointer 1 node
    pub fn sr_cursor_sp(&mut self) -> u32 {
        let current_node = unsafe { &mut *(self.head as *mut Node) };
        self.head = current_node.next;
        let tail_node = unsafe { &mut *(self.tail as *mut Node) };
        self.tail = tail_node.next;
        let node_new = unsafe { &mut *(self.head as *mut Node) };
        let t = &mut *(&mut node_new.data as &mut TCB);
        t.sp
    }
    // returns whole tcb
    pub fn _sr_cursor(&mut self) -> u32 {
        let node = unsafe { &mut *(self.cursor as *mut Node) };
        self.cursor = node.next;
        let node_new = unsafe { &mut *(node.next as *mut Node) };
        let t = &mut *(&mut node_new.data as &mut TCB);
        core::ptr::addr_of!(*t) as *const u32 as u32
    }
    pub fn update_tcb(&mut self, value: u32) {
        let node = unsafe { &mut *(self.head as *mut Node) };
        node.data.sp = value;
    }
}

use crate::mem::malloc::get_mem;

use super::super::proc::tcb::TCB;
// use super::super::mem::malloc;
#[repr(C)]
struct Node {
    data: TCB,
    next: *const u32,
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
        } else {
            node.next = self.head;
            let tail_node = unsafe { &mut *(self.tail as *mut Node) };
            tail_node.next = mem_for_new_node as *const u32;
            self.head = mem_for_new_node as *const u32;
        }
        self.size += 1;
        // self.head = mem_for_new_node as *const u32;
    }
    pub fn next(&mut self) -> u32 {
        if self.cursor == core::ptr::null() {
            self.cursor = self.head;
        } else {
            let node = unsafe { &mut *(self.cursor as *mut Node) };
            self.cursor = node.next;
        }
        let test = unsafe { &mut *(self.cursor as *mut Node) };
        self.cursor as u32
        // let current = unsafe { &mut *(self.cursor as *mut Node) };
        //
        // let list = unsafe { &mut *(self.cursor as *mut List) };
        // node.next as u32
    }
}

use super::super::proc::tcb::TCB;
use crate::mem::malloc::{free, get_mem};
#[repr(C)]
struct Node<'a> {
    next: *const u32,
    data: TCB<'a>,
}

#[repr(C)]
pub struct List {
    size: u32,
    head: *const u32,
    tail: *const u32,
}

impl List {
    pub fn new() -> u32 {
        unsafe {
            let mem_start = get_mem(core::mem::size_of::<List>() as u32);
            let list = &mut *(mem_start as *mut List);
            *list = List {
                size: 0,
                head: core::ptr::null(),
                tail: core::ptr::null(),
            };
            mem_start
        }
    }
    pub fn delete_head_node(&mut self) {
        let addr_of_old_head = self.head;
        unsafe {
            let head = &mut *(self.head as *mut Node);
            let tail = &mut *(self.tail as *mut Node);
            tail.next = head.next;
            self.head = head.next;
            self.size -= 1;
            free(addr_of_old_head as u32);
        }
    }
    pub fn get_size(&mut self) -> u32 {
        self.size
    }
    pub fn insert(&mut self, tcb: TCB) {
        unsafe {
            let mem_for_new_node = get_mem(core::mem::size_of::<Node>() as u32);
            let node = &mut *(mem_for_new_node as *mut Node);

            *node = Node {
                data: tcb,
                next: core::ptr::null(),
            };

            if self.head.is_null() {
                node.next = mem_for_new_node as *const u32;
                self.tail = mem_for_new_node as *const u32;
                self.head = mem_for_new_node as *const u32;
            } else {
                // append to tail / fifo
                node.next = self.head;
                let tail_node = &mut *(self.tail as *mut Node);
                tail_node.next = mem_for_new_node as *const u32;
                self.tail = mem_for_new_node as *const u32;
            }
            self.size += 1;
        }
    }
    // returns sp only and shifts pointer 1 node
    pub fn get_next_sp(&mut self) -> u32 {
        let current_node = unsafe { &mut *(self.head as *mut Node) };
        self.head = current_node.next;
        let tail_node = unsafe { &mut *(self.tail as *mut Node) };
        self.tail = tail_node.next;
        let node_new = unsafe { &mut *(self.head as *mut Node) };
        let t = &mut *(&mut node_new.data as &mut TCB);
        t.sp
    }
    // returns current sp
    pub fn cursor_sp(&mut self) -> u32 {
        let node_new = unsafe { &mut *(self.head as *mut Node) };
        let t = &mut *(&mut node_new.data as &mut TCB);
        t.sp
    }
    pub fn update_tcb(&mut self, value: u32) {
        // unsafe {
        //     core::intrinsics::volatile_store((self.head as u32 + 0x1C) as *mut u32, value);
        // }
        let node = unsafe { &mut *(self.head as *mut Node) };
        node.data.sp = value;
    }
}

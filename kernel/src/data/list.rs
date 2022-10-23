use super::super::proc::tcb::TCB;
use crate::mem::malloc::{allocate, MemoryResult};
use core::cell::Cell;

#[repr(C)]
struct Node<T> {
    next: Cell<u32>,
    data: T,
}

impl<T> Node<T> {
    pub fn foo(&mut self) {
        todo!();
    }
}

#[repr(C)]
pub struct List {
    size: u32,
    head: Cell<u32>,
    tail: Cell<u32>,
}

impl List {
    pub fn new() -> u32 {
        unsafe {
            let mem_start: Option<MemoryResult> = allocate(core::mem::size_of::<List>() as u32);
            match mem_start {
                Some(result) => {
                    let list = &mut *(result.start_address as *mut List);
                    *list = List {
                        size: 0,
                        head: Cell::new(0),
                        tail: Cell::new(0),
                    };
                    result.start_address
                }
                None => {
                    todo!();
                }
            }
        }
    }

    pub fn task_queue_get_size(&self) -> u32 {
        self.size
    }

    pub fn delete_head_node(&mut self) {
        //let addr_of_old_head = self.head;
        todo!();
        // unsafe {
        //     let head = &mut *(self.head as *mut Node<T>);
        //     let tail = &mut *(self.tail as *mut Node<T>);
        //     tail.next = head.next;
        //     self.head = head.next;
        //     self.size -= 1;
        //     // free(addr_of_old_head as u32);
        // }
    }
    pub fn insert<T>(&mut self, data: T) {
        unsafe {
            let mem_for_new_node = allocate(core::mem::size_of::<Node<T>>() as u32);
            match mem_for_new_node {
                Some(result) => {
                    let node = &mut *(result.start_address as *mut Node<T>);

                    *node = Node {
                        data,
                        next: Cell::new(0),
                    };

                    if self.head.get() == 0 {
                        *node.next.get_mut() = result.start_address;
                        *self.tail.get_mut() = result.start_address;
                        *self.head.get_mut() = result.start_address;
                    } else {
                        // append to tail / fifo
                        *node.next.get_mut() = self.head.get();
                        let tail_node: &mut Node<T> = &mut *(self.tail.get() as *mut Node<T>);
                        *tail_node.next.get_mut() = result.start_address;
                        *self.tail.get_mut() = result.start_address;
                    }
                    self.size += 1;
                }
                None => {
                    todo!()
                }
            }
        }
    }
    // returns sp only and shifts pointer 1 node
    pub fn sr_cursor_sp(&mut self) -> u32 {
        let current_node = unsafe { &mut *(self.head.get() as *mut Node<TCB>) };
        *self.head.get_mut() = current_node.next.get();
        let tail_node = unsafe { &mut *(self.tail.get() as *mut Node<TCB>) };
        *self.tail.get_mut() = tail_node.next.get();
        let node_new = unsafe { &mut *(self.head.get() as *mut Node<TCB>) };
        let t = &mut *(&mut node_new.data as &mut TCB);
        t.sp
        // 0
    }
    // returns current sp
    pub fn cursor_sp(&mut self) -> u32 {
        // let node_new = unsafe { &mut *(self.head as *mut Node) };
        // let t = &mut *(&mut node_new.data as &mut TCB);
        // t.sp
        0
    }
    // // returns whole tcb
    // pub fn _sr_cursor(&mut self) -> u32 {
    //     let node = unsafe { &mut *(self.cursor as *mut Node) };
    //     self.cursor = node.next;
    //     let node_new = unsafe { &mut *(node.next as *mut Node) };
    //     let t = &mut *(&mut node_new.data as &mut TCB);
    //     core::ptr::addr_of!(*t) as *const u32 as u32
    // }
    pub fn update_tcb(&mut self, value: u32) {
        // let node = unsafe { &mut *(self.head as *mut Node) };
        // node.data.sp = value;
    }
    pub fn update_tcb_and_shift_task(&mut self, value: u32) -> u32 {
        // let current_node = unsafe { &mut *(self.head as *mut Node) };
        // current_node.data.sp = value;
        // self.head = current_node.next;
        // let tail_node = unsafe { &mut *(self.tail as *mut Node) };
        // self.tail = tail_node.next;
        // let node_new = unsafe { &mut *(self.head as *mut Node) };
        // let t = &mut *(&mut node_new.data as &mut TCB);
        // t.sp
        0
    }
}

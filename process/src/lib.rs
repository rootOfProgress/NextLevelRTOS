#![no_std]
extern crate cpu;
pub mod blueprint;

pub fn new_process(target: u32, end_destination: u32) -> Option<blueprint::Frame> {
    let cpu_register = cpu::core::CoreRegister::default();
    let mut process = blueprint::Frame::new(cpu_register);
    process = match process {
        Some(mut p) => {
            p.set_target_addr(target);
            p.set_end_destination_addr(end_destination);
            Some(p)
        }
        None => None,
    };
    process
}

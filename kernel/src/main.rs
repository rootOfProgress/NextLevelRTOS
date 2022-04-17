//!
//! Main crate for Kernelimplementation. Depends on runtime
//! and the main function gets called after successful power on.
//!
#![no_std]
#![no_main]
#![feature(asm)]
#![feature(core_intrinsics)]
extern crate cpu;
extern crate devices;
extern crate runtime;
mod data;
mod mem;
mod proc;
use devices::controller::uart::iostream;
use devices::generic::platform::stm32f407ve::adresses;
use devices::io::gpio::gpio::GpioDevice;
use proc::sched;

fn fibonacci(n: u32) -> u32 {
    match n {
        0 => 1,
        1 => 1,
        _ => fibonacci(n - 1) + fibonacci(n - 2),
    }
}

fn led_off() {
    loop {
        // "task0".println();
    }
    // loop {
    // unsafe {
    // let mut reg_content = core::ptr::read_volatile(0x4002_0014 as *mut u32);
    // reg_content &= !((0b1_u32) << 2);
    // core::ptr::write_volatile(0x4002_0014 as *mut u32, reg_content);
    // }
    // }
}

fn led_on() {
    loop {
        // "task1".println();
    }
    // loop {
    // unsafe {
    // let mut reg_content = core::ptr::read_volatile(0x4002_0014 as *mut u32);
    // reg_content |= (0b1_u32) << 2;
    // core::ptr::write_volatile(0x4002_0014 as *mut u32, reg_content);
    // }
    // }
}

fn calculate_fibonacci() {
    // "hello fibu".println();
    loop {
        // "task2".println();
    }
    // run 1 time, then destroy
    //fibonacci(22);
}

fn user_init() {
    sched::spawn_task(calculate_fibonacci as *const u32 as u32, "fibu", 128);
    sched::spawn_task(led_on as *const u32 as u32, "fibu", 128);
    sched::spawn_task(led_off as *const u32 as u32, "fibu", 128);
    sched::spawn_task(led_on as *const u32 as u32, "fibu", 128);
    sched::spawn_task(led_off as *const u32 as u32, "fibu", 128);
    sched::spawn_task(led_on as *const u32 as u32, "fibu", 128);
    sched::spawn_task(led_off as *const u32 as u32, "fibu", 128);
    sched::spawn_task(led_on as *const u32 as u32, "fibu", 128);
    sched::spawn_task(led_off as *const u32 as u32, "fibu", 128);
    sched::spawn_task(led_on as *const u32 as u32, "fibu", 128);
    sched::spawn_task(led_off as *const u32 as u32, "fibu", 128);
    sched::spawn_task(led_on as *const u32 as u32, "fibu", 128);
    sched::spawn_task(led_off as *const u32 as u32, "fibu", 128);
    sched::spawn_task(led_on as *const u32 as u32, "fibu", 128);
    sched::spawn_task(led_off as *const u32 as u32, "fibu", 128);
    sched::spawn_task(led_on as *const u32 as u32, "fibu", 128);
    sched::spawn_task(led_off as *const u32 as u32, "fibu", 128);
    sched::spawn_task(led_on as *const u32 as u32, "fibu", 128);
    sched::spawn_task(led_off as *const u32 as u32, "fibu", 128);
    sched::spawn_task(led_on as *const u32 as u32, "fibu", 128);
    sched::spawn_task(led_off as *const u32 as u32, "fibu", 128);

    
    // let mut r = 0;
    // for i in 0..460 {
        // unsafe {
// 
            // r = mem::malloc::get_mem(4);
        // }
    // }
    // unsafe {
    //     let cpu_register_mem =
    //         mem::malloc::get_mem(core::mem::size_of::<cpu::core::CoreRegister>() as u32 + 256);
    //     let s = core::mem::size_of::<cpu::core::CoreRegister>() as u32;
    //     // let x = cpu_register_mem + 256;
    //     let cr = &mut *(((cpu_register_mem + 256)
    //         - core::mem::size_of::<cpu::core::CoreRegister>() as u32)
    //         as *mut cpu::core::CoreRegister);
    //     cr.r4 = 0x123;
    //     cr.pc = calculate_fibonacci as *const () as u32;
    //     cr.lr = sched::destroy as *const () as u32;
    //     cr.psr = 0x21000000;
    //     let addy = core::ptr::addr_of!(cr.r4);
    //     let b = 0;
    //     "spawn process 1".println();
    //     sched::spawn1(addy as u32, "calculate_fibonacci");
    // }
    // let mut process = blueprint::Frame::new(cpu_register);
    // process = match process {
    //     Some(mut p) => {
    //         p.set_target_addr(target);
    //         p.set_end_destination_addr(end_destination);
    //         Some(p)
    //     }
    //     None => None,
    // };

    // let led_off = process::new_process(
    //     led_off as *const () as u32,
    //     sched::destroy as *const () as u32,
    // )
    // .unwrap();
    // let led_on = process::new_process(
    //     led_on as *const () as u32,
    //     sched::destroy as *const () as u32,
    // )
    // .unwrap();
    // sched::spawn(calculate_fibo, "calculate_fibonacci");
    // "spawn process 2".println();
    // sched::spawn(led_off, "led_off");
    // "spawn process 3".println();
    // sched::spawn(led_on, "led_on");
    // "usooor".println();
    loop {
       // sched::sleep();
    }
}

///
/// Target function after hardware initialization,
/// acts as the first kernel function.
///
#[no_mangle]
pub unsafe fn kernel_init() -> ! {
    mem::malloc::init();
    sched::init();
    devices::sys::tick::init_systick(1280);

    let gpio_port_a2 = devices::io::gpio::gpio::GpioDevice::new("A", 2)
        .as_output()
        .as_push_pull();
    gpio_port_a2.turn_on();

    let gpio_port_a3 = devices::io::gpio::gpio::GpioDevice::new("A", 3)
        .as_output()
        .as_push_pull();
    gpio_port_a3.turn_on();

    GpioDevice::new("A", 9)
        .as_alternate_function()
        .as_push_pull()
        .as_af(7);

    let usart = devices::controller::uart::usart::UsartDevice::new(9600);
    usart.enable();

    // let early_user_land =
        // process::new_process(user_init as *const () as u32, user_init as *const () as u32).unwrap();

    // "hello from trait".println();
    // "usart works without errors...".println();
    // sched::spawn(early_user_land, "early_user_land");
    sched::spawn_task(user_init as *const u32 as u32, "init", 512);
    sched::start_init_process();

    loop {
        "!! KERNEL PANIC - NO INIT FOUND !!".println();
        asm!("bkpt");
    }
}

//!
//! Main crate for Kernelimplementation. Depends on runtime
//! and the main function gets called after successful power on.
//!
#![no_std]
#![no_main]
#![feature(asm)]
#![feature(core_intrinsics)]
extern crate devices;
extern crate process;
extern crate runtime;
mod data;
mod mem;
mod proc;
use devices::controller::timer::tim::TimerDevice;
use devices::controller::uart::iostream;
use devices::generic::platform::stm32f3x::adresses;
use devices::generic::platform::stm32f3x::bitfields;
use devices::io::gpio::gpio::GpioDevice;
use proc::sched;
use user::engine;

static mut ENGINE_OUT: Option<TimerDevice> = None;

fn fibonacci(n: u32) -> u32 {
    match n {
        0 => 1,
        1 => 1,
        _ => fibonacci(n - 1) + fibonacci(n - 2),
    }
}

fn led_off() {
    loop {
        unsafe {
            let mut reg_content = core::ptr::read_volatile(0x4002_0014 as *mut u32);
            reg_content &= !((0b1_u32) << 2);
            core::ptr::write_volatile(0x4002_0014 as *mut u32, reg_content);
        }
    }
}

fn led_on() {
    loop {
        unsafe {
            let mut reg_content = core::ptr::read_volatile(0x4002_0014 as *mut u32);
            reg_content |= (0b1_u32) << 2;
            core::ptr::write_volatile(0x4002_0014 as *mut u32, reg_content);
        }
    }
}

fn calculate_fibonacci() {
    // loop {
    fibonacci(22);
    // }
}

fn user_init() {
    let calculate_fibonacci = process::new_process(
        calculate_fibonacci as *const () as u32,
        sched::destroy as *const () as u32,
    )
    .unwrap();
    let led_off = process::new_process(
        led_off as *const () as u32,
        sched::destroy as *const () as u32,
    )
    .unwrap();
    let led_on = process::new_process(
        led_on as *const () as u32,
        sched::destroy as *const () as u32,
    )
    .unwrap();
    "spawn process 1".println();
    sched::spawn(1, calculate_fibonacci, "calculate_fibonacci");
    "spawn process 2".println();
    sched::spawn(2, led_off, "led_off");
    "spawn process 3".println();
    sched::spawn(3, led_on, "led_on");
    loop {}
}

// pub unsafe fn get_out_0() -> &'a TimerDevice {
//     let mut tim_2 = &TimerDevice::new(2);
//     tim_2.set_arr_register(1000)
//         .set_psc_register(7)
//         .set_ccmr1_register((6 << 12 )| (1 << 11)) // enable preload
//         .set_cr1_register(1 << 7) // enable autoreload preload
//         .set_ccer_register(1 << 4) // enable channel 2 output
//         .set_egr_register(1)
//         .set_cr1_register(1); // enable

//     tim_2
// }

unsafe fn get_tim_device() -> &'static mut TimerDevice {
    match ENGINE_OUT {
        Some(ref mut x) => &mut *x,
        None => panic!(),
    }
}

unsafe fn alter_speed(engine_number: u32, value: u32) {
    let mut timer_device: &'static mut TimerDevice = get_tim_device();
    match engine_number {
        1 => *timer_device = timer_device.set_ccr1_register(value),
        2 => *timer_device = timer_device.set_ccr2_register(value),
        3 => *timer_device = timer_device.set_ccr3_register(value),
        4 => *timer_device = timer_device.set_ccr4_register(value),
        _ => panic!(),
    }
}

unsafe fn init_tim_3() {
    ENGINE_OUT = Some(TimerDevice::new(3));
    let mut baz: &'static mut TimerDevice = get_tim_device();
    *baz = baz
        .set_arr_register(12)
        .set_psc_register(7)
        .set_ccmr1_register(
            (6 << bitfields::tim3::ccmr1::OC2M)
                | (1 << bitfields::tim3::ccmr1::OC2PE)
                | (6 << bitfields::tim3::ccmr1::OC1M)
                | (1 << bitfields::tim3::ccmr1::OC1PE),
        ) // enable preload
        .set_ccmr2_register(
            (6 << bitfields::tim3::ccmr2::OC4M)
                | (1 << bitfields::tim3::ccmr2::OC4PE)
                | (6 << bitfields::tim3::ccmr2::OC3M)
                | (1 << bitfields::tim3::ccmr2::OC3PE),
        ) // enable preload
        .set_cr1_register(1 << bitfields::tim3::cr1::ARPE) // enable autoreload preload
        .set_ccer_register(
            1 << bitfields::tim3::ccer::CC1E
                | 1 << bitfields::tim3::ccer::CC2E
                | 1 << bitfields::tim3::ccer::CC3E
                | 1 << bitfields::tim3::ccer::CC4E,
        ) // enable channel 2 output
        .set_egr_register(1)
        .set_cr1_register(1 << bitfields::tim3::cr1::CEN); // enable
}

///
/// Target function after hardware initialization,
/// acts as the first kernel function.
///
#[no_mangle]
pub unsafe fn kernel_init() -> ! {
    mem::malloc::init();
    sched::init();
    devices::sys::tick::init_systick(280);
    let mut foo = 200;
    let gpio_port_a0 = devices::io::gpio::gpio::GpioDevice::new("A", 0)/* .as_input() */;

    // tim2
    // let gpio_port_a1 = devices::io::gpio::gpio::GpioDevice::new("A", 1)
    //     .as_alternate_function()
    //     .as_af(1);

    // // tim3
    // let gpio_port_a4 = devices::io::gpio::gpio::GpioDevice::new("A", 4)
    //     .as_alternate_function()
    //     .as_af(2);

    // tim15
    // let gpio_port_a2 = devices::io::gpio::gpio::GpioDevice::new("A", 2)
    //     .as_alternate_function()
    //     .as_af(1);
    init_tim_3();
    let gpio_port_c6 = devices::io::gpio::gpio::GpioDevice::new("C", 6)
        .as_alternate_function()
        .as_af(2);
    let gpio_port_c7 = devices::io::gpio::gpio::GpioDevice::new("C", 7)
        .as_alternate_function()
        .as_af(2);
    let gpio_port_c8 = devices::io::gpio::gpio::GpioDevice::new("C", 8)
        .as_alternate_function()
        .as_af(2);
    let gpio_port_c9 = devices::io::gpio::gpio::GpioDevice::new("C", 9)
        .as_alternate_function()
        .as_af(2);
    // gpio_port_a2.turn_on();

    // ENGINE_OUT_0 = Some(TimerDevice::new(2)).set_arr_register(1000)
    //                  .set_psc_register(7)
    //                  .set_ccmr1_register((6 << 12 )| (1 << 11)) // enable preload
    //                  .set_cr1_register(1 << 7) // enable autoreload preload
    //                  .set_ccer_register(1 << 4) // enable channel 2 output
    //                  .set_egr_register(1)
    //                  .set_cr1_register(1); // enable

    // ENGINE_OUT_0 = Some(TimerDevice::new(2));
    // init_t15();
    // init_t2();
    // let mut baz: &'static mut TimerDevice = ctx_0();
    // *baz = baz
    //     .set_arr_register(12)
    //     .set_psc_register(7)
    //     .set_ccmr1_register((6 << 12) | (1 << 11)) // enable preload
    //     .set_cr1_register(1 << 7) // enable autoreload preload
    //     .set_ccer_register(1 << 4) // enable channel 2 output
    //     .set_egr_register(1)
    //     .set_cr1_register(1); // enable

    //
    // let gpio_port_a3 = devices::io::gpio::gpio::GpioDevice::new("A", 3)
    // .as_output()
    // .as_push_pull();
    // gpio_port_a3.turn_on();
    //
    // GpioDevice::new("A", 9)
    // .as_alternate_function()
    // .as_push_pull()
    // .as_af(7);

    // let usart = devices::controller::uart::usart::UsartDevice::new(9600);
    // usart.enable();

    // let early_user_land =
    // process::new_process(user_init as *const () as u32, user_init as *const () as u32).unwrap();
    //
    // "hello from trait".println();
    // "usart works without errors...".println();
    // sched::spawn(0, early_user_land, "early_user_land");
    // sched::start_init_process();
    let mut x: u32 = 0;
    loop {
        if (gpio_port_a0.is_pressed()) {
            // asm!("bkpt");
            if (x < 300) {
                x += 1;
            }
        } else {
            if (x > 1) {
                x -= 1;
            }
        }
        engine::alter_engine_speed(ENGINE_OUT, 1, x);
        engine::alter_engine_speed(ENGINE_OUT, 2, x);
        engine::alter_engine_speed(ENGINE_OUT, 3, x);
        engine::alter_engine_speed(ENGINE_OUT, 4, x);
        // alter_speed(1, x);
        // alter_speed(1, x);
        // alter_speed(2, x);
        // alter_speed(x);
        for i in 0..10000 {}
        // if (x > 300) {
        //     x -= 1;
        // } else if (x < 50) {
        //     x += 1;
        // }
        // ENGINE_OUT_0 = Some(ENGINE_OUT_0.unwrap().set_ccr2_register(80));

        // "!! KERNEL PANIC - NO INIT FOUND !!".println();
        // asm!("bkpt");
    }
}

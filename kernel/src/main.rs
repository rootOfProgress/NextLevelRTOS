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
use devices::io::i2c::i2c::I2cDevice;
use proc::sched;
use user::engine;
use devices::controller::uart::res;
static mut TIM_3: Option<TimerDevice> = None;

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
unsafe fn init_tim_3() {
    TIM_3 = Some(TimerDevice::new(3));
    let mut tim3: &'static mut TimerDevice = match TIM_3 {
        Some(ref mut x) => &mut *x,
        None => panic!(),
    };
    *tim3 = tim3
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
    // devices::sys::tick::init_systick(280);

    let gpio_port_a0 = devices::io::gpio::gpio::GpioDevice::new("A", 0);

    // i2c1 sda
    // devices::io::gpio::gpio::GpioDevice::new("A", 14)
    //     .as_alternate_function()
    //     .as_af(4)
    //     .as_open_drain()
    //     .as_push_pull()
    //     // .as_high_speed()
    //     .as_pull_up();

    // // i2c1 scl
    // devices::io::gpio::gpio::GpioDevice::new("A", 15)
    //     .as_alternate_function()
    //     .as_af(4)
    //     .as_open_drain()
    //     .as_push_pull()
    //     // .as_high_speed()
    //     .as_pull_up();

    // devices::io::i2c::i2c::I2cDevice::new().init();


    // // uart1 tx
    devices::io::gpio::gpio::GpioDevice::new("B", 6)
        .as_alternate_function()
        .as_af(7)
        .as_push_pull();
    // // uart1 rx
    devices::io::gpio::gpio::GpioDevice::new("B", 7).as_input().as_alternate_function().as_af(7);

    // // speed regulation
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

    let usart = devices::controller::uart::usart::UsartDevice::new(9600);
    usart.enable();
    "hello from trait".println();

    // let mut x: u32 = 0;
    loop {
        // if (gpio_port_a0.is_pressed()) {
        //     if (x < 300) {
        //         x += 1;
        //     }
        // } else {
        //     if (x > 1) {
        //         x -= 1;
        //     }
        // }
        // engine::alter_engine_speed(TIM_3, 2);
        // engine::alter_engine_speed(TIM_3, 2, x);
        // engine::alter_engine_speed(TIM_3, 3, x);
        // engine::alter_engine_speed(TIM_3, 4, x);

        // for i in 0..10000 {}
    }
}

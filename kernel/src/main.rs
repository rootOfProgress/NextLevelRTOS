//!
//! Main crate for Kernelimplementation. Depends on runtime
//! and the main function gets called after successful power on.
//!
#![no_std]
#![no_main]
#![feature(asm)]
#![feature(core_intrinsics)]
extern crate devices;
extern crate runtime;
extern crate user;

mod data;
mod mem;
mod proc;
use devices::controller::timer::tim::TimerDevice;
use devices::controller::uart::iostream;
use proc::task::task::create_task;
use user::engine::alter_engine_speed;
use devices::generic::platform::stm32f3x::bitfields;


use devices::io::i2c::i2c::I2C1_DEV;
use proc::sched;

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
        // unsafe {
        //     let mut reg_content = core::ptr::read_volatile(0x4002_0014 as *mut u32);
        //     reg_content &= !((0b1_u32) << 2);
        //     core::ptr::write_volatile(0x4002_0014 as *mut u32, reg_content);
        // }
    }
}

fn led_on() {
    loop {
        // unsafe {
        //     let mut reg_content = core::ptr::read_volatile(0x4002_0014 as *mut u32);
        //     reg_content |= (0b1_u32) << 2;
        //     core::ptr::write_volatile(0x4002_0014 as *mut u32, reg_content);
        // }
    }
}

fn calculate_fibonacci() {
    // loop {
    fibonacci(22);
    // }
}

fn user_init() {
    let calculate_fibonacci = create_task(
        calculate_fibonacci as *const () as u32,
        sched::destroy as *const () as u32,
    )
    .unwrap();
    let led_off = create_task(
        led_off as *const () as u32,
        sched::destroy as *const () as u32,
    )
    .unwrap();
    let led_on = create_task(
        led_on as *const () as u32,
        sched::destroy as *const () as u32,
    )
    .unwrap();
    sched::spawn(1, calculate_fibonacci, "calculate_fibonacci");
    sched::spawn(2, led_off, "led_off");
    sched::spawn(3, led_on, "led_on");
    loop {}
}
unsafe fn init_tim_3() {
    TIM_3 = Some(TimerDevice::new(3));
    let tim3: &'static mut TimerDevice = match TIM_3 {
        Some(ref mut x) => &mut *x,
        None => panic!(),
    };
    *tim3 = tim3
        .set_arr_register(100)
        .set_psc_register(2)
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

    // let gpio_port_a0 = devices::io::gpio::gpio::GpioDevice::new("A", 0);

    // // i2c1 sda
    devices::io::gpio::gpio::GpioDevice::new("B", 7)
        .as_af(4)
        .as_alternate_function()
        .as_open_drain()
        .as_high_speed();
    // // i2c1 scl
    devices::io::gpio::gpio::GpioDevice::new("B", 8)
        .as_af(4)
        .as_alternate_function()
        .as_open_drain()
        .as_high_speed();

    I2C1_DEV = Some(devices::io::i2c::i2c::I2cDevice::new().init());
    
    
    devices::io::gpio::gpio::GpioDevice::new("A", 0).as_output().turn_off();
    devices::io::gpio::gpio::GpioDevice::new("A", 1).as_output().turn_off();

    // // uart1 tx
    devices::io::gpio::gpio::GpioDevice::new("A", 9)
        .as_alternate_function()
        .as_af(7)
        .as_push_pull();
    // // uart1 rx
    devices::io::gpio::gpio::GpioDevice::new("A", 10)
        .as_input()
        .as_alternate_function()
        .as_af(7);

    // // speed regulation
    init_tim_3();

    let _gpio_port_c6 = devices::io::gpio::gpio::GpioDevice::new("C", 6)
        .as_alternate_function()
        .as_af(2);
    let _gpio_port_c7 = devices::io::gpio::gpio::GpioDevice::new("C", 7)
        .as_alternate_function()
        .as_af(2);
    let _gpio_port_c8 = devices::io::gpio::gpio::GpioDevice::new("C", 8)
        .as_alternate_function()
        .as_af(2);
    let _gpio_port_c9 = devices::io::gpio::gpio::GpioDevice::new("C", 9)
        .as_alternate_function()
        .as_af(2);

    let usart = devices::controller::uart::usart::UsartDevice::new(9600);
    usart.enable();
    "hello from trait".println();
   // devices::sys::tick::init_systick(280);
    // let init =
        // create_task(user_init as *const () as u32, user_init as *const () as u32).unwrap();
// 
    // sched::spawn(0, init, "init");
    // sched::start_init_process();
    // let mut x: u32 = 0;
    loop {
        alter_engine_speed(TIM_3, 0);
    }
}

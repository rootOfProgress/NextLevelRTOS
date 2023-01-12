#![no_std]
#![feature(core_intrinsics)]
// #![feature(asm)]
pub mod bus;
pub mod controller;
pub mod generic;
pub mod io;
pub mod registerblocks;
pub mod sys;

use io::gpio::gpio::GpioDevice;
use io::i2c::i2c::I2cDevice;
use controller::timer::tim::TimerDevice;
use generic::platform::stm32f3x::bitfields;


unsafe fn init_tim_3() {

    let mut tim_3: TimerDevice = TimerDevice::new(3);
    
    tim_3
        .set_arr_register(999)
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


#[no_mangle] 
pub unsafe extern "C" fn init_devs ()
{
    GpioDevice::new("B", 7)
        .as_af(4)
        .as_alternate_function()
        .as_open_drain()
        .as_high_speed();
    // // i2c1 scl
    GpioDevice::new("B", 8)
        .as_af(4)
        .as_alternate_function()
        .as_open_drain()
        .as_high_speed();

    I2cDevice::new().init();

    //GpioDevice::new("A", 3).as_input();

    // // speed regulation
    init_tim_3();

    let _gpio_port_c6 = GpioDevice::new("C", 6)
        .as_alternate_function()
        .as_af(2);
    let _gpio_port_c7 = GpioDevice::new("C", 7)
        .as_alternate_function()
        .as_af(2);
    let _gpio_port_c8 = GpioDevice::new("C", 8)
        .as_alternate_function()
        .as_af(2);
    let _gpio_port_c9 = GpioDevice::new("C", 9)
        .as_alternate_function()
        .as_af(2);
    // loop{}
}
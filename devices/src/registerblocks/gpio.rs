#[repr(C)]
pub struct GPIO {
    pub moder: u32,
    pub otyper: u32,
    pub ospeedr: u32,
    pub pupdr: u32,
    pub idr: u32,
    pub odr: u32,
    pub bsrr: u32,
    pub lckr: u32,
    pub afrl: u32,
    pub afrh: u32,
    pub brr: u32,
}

impl GPIO {
    pub fn new(gpio_base_adress: u32) -> &'static GPIO {
        unsafe { &mut *(gpio_base_adress as *mut GPIO) }
    }
}
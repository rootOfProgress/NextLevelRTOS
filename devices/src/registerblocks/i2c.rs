#[repr(C)]
pub struct I2C {
    pub CR1: u32,
    pub CR2: u32,
    pub OAR1: u32,
    pub OAR2: u32,
    pub TIMINGR: u32,
    pub TIMEOUTR: u32,
    pub ISR: u32,
    pub ICR: u32,
    pub PECR: u32,
    pub RXDR: u32,
    pub TXDR: u32,
}

//---------------------------------------------------------------//
//---------------------STRUCT-IMPLEMENTATIONS--------------------//
//---------------------------------------------------------------//
impl I2C {
    pub fn new(i2c_base_adress: u32) -> &'static I2C {
        unsafe { &mut *(i2c_base_adress as *mut I2C) }
    }
}

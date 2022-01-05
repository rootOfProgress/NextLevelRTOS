//!
//! This file contains a struct containing the registers for the rcc bus. The fields of the struct are in C presentation
//! to prevent compiler mangling. The fields then match the offsets of the according register.
//!
#[repr(C)]
pub struct RCC {
    pub cr: u32,
    pub cfgr: u32,
    pub cir: u32,
    pub apb2rstr: u32,
    pub apb1rstr: u32,
    pub ahbenr: u32,
    pub apb2enr: u32,
    pub apb1enr: u32,
    pub bdcr: u32,
    pub csr: u32,
    pub ahbrstr: u32,
    pub cfgr2: u32,
    pub cfgr3: u32,
}

impl RCC {
    ///
    /// Returns a new RCC Struct based on the registers base adress. This adress gets
    /// casted to the struct, as a result the first field will equals the base
    /// adress. The following ones are stacked ontop each other with an offset of
    /// 4 byte / 32 bit.
    ///
    pub fn new(rcc_base_adress: u32) -> *mut RCC {
        unsafe { &mut *(rcc_base_adress as *mut RCC) }
    }
}

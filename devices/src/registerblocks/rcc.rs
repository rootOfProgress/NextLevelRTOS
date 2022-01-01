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
    pub fn new(rcc_base_adress: u32) -> *mut RCC {
        unsafe { &mut *(rcc_base_adress as *mut RCC) }
    }
}
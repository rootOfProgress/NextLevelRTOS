#[repr(C)]
pub struct TIMx {
    pub CR1: u32,
    pub CR2: u32,
    pub SMCR: u32,
    pub DIER: u32,
    pub SR: u32,
    pub EGR: u32,   // 0x14
    pub CCMR1: u32, // 0x18
    pub CCMR2: u32,
    pub CCER: u32, // 0x20
    pub CNT: u32,
    pub PSC: u32, // 0x28
    pub ARR: u32,
    pub RESERVED: u32, // 0x30
    pub CCR1: u32,
    pub CCR2: u32,
    pub CCR3: u32,
    pub CCR4: u32,
    pub DCR: u32,
    pub DMAR: u32,
}

//---------------------------------------------------------------//
//---------------------STRUCT-IMPLEMENTATIONS--------------------//
//---------------------------------------------------------------//
impl TIMx {
    ///
    /// Returns a new GPIO Struct based on the registers base adress. This adress gets
    /// casted to the struct, as a result the first field will equals the base
    /// adress. The following ones are stacked ontop each other with an offset of
    /// 4 byte / 32 bit.
    ///
    pub fn new(tim_base_adress: u32, tim_number: u32) -> &'static TIMx {
        // match tim_number {
        // 2 | 3 | 4 => unsafe { &mut *(tim_base_adress as *mut TIMx) },
        // _ => panic!()
        unsafe { &mut *(tim_base_adress as *mut TIMx) }
        //}
    }
}

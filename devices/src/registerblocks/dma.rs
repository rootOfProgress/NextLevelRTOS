//!
//! This file contains a struct containing the registers for the DMA device. The fields of the struct are in C presentation 
//! to prevent compiler mangling. The fields then match the offsets of the according register.
//!
#[repr(C)]
pub struct DMA {
    pub isr: u32,
    pub ifcr: u32,
    pub ccrx: u32,
    pub cndtrx: u32,
    pub cparx: u32,
    pub cmarx: u32
}

impl DMA {
    ///
    /// Returns a new DMA Struct based on the registers base adress. This adress gets
    /// casted to the struct, as a result the first field will equals the base
    /// adress. The following ones are stacked ontop each other with an offset of
    /// 4 byte / 32 bit.
    ///
    pub fn new(dma_base_adress: u32) -> &'static DMA {
        unsafe { &mut *(dma_base_adress as *mut DMA) }
    }
}
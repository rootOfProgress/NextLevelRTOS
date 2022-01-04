//!
//! This file contains a struct containing the registers for the DMA device. The fields of the struct are in C presentation
//! to prevent compiler mangling. The fields then match the offsets of the according register.
//!
#[repr(C)]
pub struct DMA {
    pub stat: &'static DMAStatic,
    pub dynamic: &'static DMADynamic,
}

#[repr(C)]
pub struct DMAStatic {
    pub isr: u32,
    pub ifcr: u32,
}

#[repr(C)]
pub struct DMADynamic {
    pub ccrx: u32,
    pub cndtrx: u32,
    pub cparx: u32,
    pub cmarx: u32,
}

impl DMA {
    ///
    /// Returns a new DMA Struct based on the registers base adress. This adress gets
    /// casted to the struct, as a result the first field will equals the base
    /// adress. The following ones are stacked ontop each other with an offset of
    /// 4 byte / 32 bit.
    ///
    pub fn new(dma_base_adress: u32, channel: u32) -> DMA {
        let offset_dyn_regs = 0x08;
        let channel_offset = 0x14 * (channel - 1);
        DMA {
            stat: unsafe { &mut *(dma_base_adress as *mut DMAStatic) },
            dynamic: unsafe {
                &mut *((dma_base_adress + offset_dyn_regs + channel_offset) as *mut DMADynamic)
            },
        }
    }
}

use super::super::generic::platform;

pub mod dma {
    use super::platform::stm32f3x::{adresses, offsets};
    use core::ptr::{read_volatile, write_volatile};
    const DMA_CCRX: u32 = adresses::dma::DMA1_BASE + offsets::dma::DMA_CCR + 0x3;

    pub fn enable_dma_channel() {
        unsafe {
            write_volatile(DMA_CCRX as *mut u32, read_volatile(DMA_CCRX as *const u32) | 0b1);
        }
    }

    pub fn disable_dma_channel() {
        unsafe {
            write_volatile(DMA_CCRX as *mut u32, read_volatile(DMA_CCRX as *const u32) & !(0b1));
        }
    }

    pub fn read_from_peripherial() {
        unsafe {
            write_volatile(DMA_CCRX as *mut u32, read_volatile(DMA_CCRX as *const u32) & !(0b1 << 4));
        }
    }

    pub fn read_from_memory() {
        unsafe {
            write_volatile(DMA_CCRX as *mut u32, read_volatile(DMA_CCRX as *const u32) | 0b1 << 4);
        }
    }
}
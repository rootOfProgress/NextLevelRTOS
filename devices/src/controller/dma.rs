use super::super::generic::platform;

pub mod dma {
    use crate::generic::platform::stm32f3x::offsets::dma::DMA_CNDTR;

    use super::platform::stm32f3x::{adresses, offsets};
    use core::ptr::{read_volatile, write_volatile};
    const DMA_CCRX: u32 = adresses::dma::DMA1_BASE + offsets::dma::DMA_CCR + 0x3;
    const DMA_CMARX: u32 = adresses::dma::DMA1_BASE + offsets::dma::DMA_CMAR + 0x3;
    const DMA_CPARX: u32 = adresses::dma::DMA1_BASE + offsets::dma::DMA_CPAR + 0x3;
    const DMA_CNDTRX: u32 = adresses::dma::DMA1_BASE + offsets::dma::DMA_CNDTR + 0x3;

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

    // source
    pub fn write_memory_adress(mem_adress: u32) {
        disable_dma_channel();
        unsafe {
            write_volatile(DMA_CMARX as *mut u32, mem_adress);
        }
        enable_dma_channel();
    }

    // destination
    pub fn write_peripherial_adress(periph_adress: u32) {
        disable_dma_channel();
        unsafe {
            write_volatile(DMA_CPARX as *mut u32, periph_adress);
        }
        enable_dma_channel();
    }

    pub fn set_number_of_bytes_to_process(num_of_bytes: u32) {
        disable_dma_channel();
        unsafe {
            write_volatile(DMA_CNDTR as *mut u32, read_volatile(DMA_CNDTR as *const u32) | num_of_bytes);
        }
        enable_dma_channel();
    }
}
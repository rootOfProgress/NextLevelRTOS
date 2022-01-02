use super::super::generic::platform::stm32f3x;
use super::super::generic::traits::primitive_extensions;
use super::super::registerblocks::dma::DMA;
pub mod dma {
    use super::primitive_extensions::BitOps;
    use super::stm32f3x::adresses;
    use super::DMA;

    pub struct DmaDevice {
        device: &'static DMA,
        offset_adder: u32,
        channel: u32,
    }

    impl DmaDevice {
        pub unsafe fn new(channel: u32) -> DmaDevice {
            DmaDevice {
                device: DMA::new(adresses::dma::DMA1),
                offset_adder: 0x20 * channel - 1,
                channel,
            }
        }

        fn enable_dma_channel(&self) {
            self.device
                .ccrx
                .set_bit_with_extra_offset(self.offset_adder, 0b1);
        }

        fn disable_dma_channel(&self) {
            self.device
                .ccrx
                .clear_bit_with_extra_offset(self.offset_adder, 0b1);
        }

        fn read_from_peripherial(&self) {
            self.device
                .ccrx
                .clear_bit_with_extra_offset(self.offset_adder, 0b1 << 4);
        }

        fn read_from_memory(&self) {
            self.device
                .ccrx
                .set_bit_with_extra_offset(self.offset_adder, 0b1 << 4);
        }

        // addr to read from
        fn set_memory_adress(&self, mem_adress: u32) {
            self.device.cmarx.write_whole_register(mem_adress);
        }

        // destination
        fn set_peripherial_adress(&self, periph_adress: u32) {
            self.device.cparx.write_whole_register(periph_adress);
        }

        fn set_number_of_bytes_to_process(&self, num_of_bytes: u16) {
            self.device.cndtrx.write_whole_register(num_of_bytes as u32);
            // disable_dma_channel();
            // unsafe {
            //     write_volatile(DMA_CNDTR as *mut u32, read_volatile(DMA_CNDTR as *const u32) | num_of_bytes);
            // }
            // enable_dma_channel();
        }
    }
}

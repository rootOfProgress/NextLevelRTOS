pub mod PrimitiveExtensions {
    pub trait BitOps {
        fn get_addr(&self) -> *const u32;
        fn set_bit(&self, bit_number: u32);
        fn clear_bit(&self, bit_number: u32);
    }
    
    impl BitOps for u32 {
        fn get_addr(&self) -> *const u32 {
            unsafe {
                *core::ptr::addr_of!(self)
            }
        }
        fn set_bit(&self, bit_number: u32) {
            unsafe {
                let address = self.get_addr();
                core::ptr::write(address as *mut u32, core::ptr::read(address) | bit_number);
            }
        }
        fn clear_bit(&self, bit_number: u32) {
            unsafe {
                let address = self.get_addr();
                core::ptr::write(address as *mut u32, core::ptr::read(address) & !bit_number);
            }
        }
    }
    
}
use super::super::bus::rcc;
use super::super::generic::platform::stm32f3x;
use super::super::generic::traits::primitive_extensions;
use super::super::registerblocks::timer::TIMx;

pub mod tim {
    use super::primitive_extensions::BitOps;
    use super::rcc;
    use super::stm32f3x::adresses;
    
    use super::TIMx;

    #[derive(Copy, Clone)]
    pub struct TimerDevice {
        device: &'static TIMx,
        number: u32,
    }

    impl TimerDevice {
        pub unsafe fn new(timer_number: u32) -> TimerDevice {
            let tim_base = match timer_number {
                2 => {
                    rcc::rcc::activate_timer_bus_clock(timer_number);
                    adresses::TIM2_BASEADRESS
                }
                3 => {
                    rcc::rcc::activate_timer_bus_clock(timer_number);
                    adresses::TIM3_BASEADRESS
                }
                4 => {
                    rcc::rcc::activate_timer_bus_clock(timer_number);
                    adresses::TIM4_BASEADRESS
                }
                6 => {
                    rcc::rcc::activate_timer_bus_clock(timer_number);
                    adresses::TIM6_BASEADRESS
                }
                15 => {
                    rcc::rcc::activate_timer_bus_clock(timer_number);
                    adresses::TIM15_BASEADRESS
                }
                _ => panic!(),
            };

            TimerDevice {
                device: TIMx::new(tim_base, timer_number),
                number: timer_number,
            }
        }

        pub unsafe fn set_arr_register(self, auto_reload_value: u32) -> TimerDevice {
            self.device.ARR.replace_whole_register(auto_reload_value);
            self
        }
        pub unsafe fn set_psc_register(self, pre_scaler_value: u32) -> TimerDevice {
            self.device.PSC.replace_whole_register(pre_scaler_value);
            self
        }
        pub unsafe fn set_ccmr1_register(self, value: u32) -> TimerDevice {
            self.device.CCMR1.write_whole_register(value);
            self
        }
        pub unsafe fn set_ccmr2_register(self, value: u32) -> TimerDevice {
            self.device.CCMR2.write_whole_register(value);
            self
        }
        pub unsafe fn set_cr1_register(self, value: u32) -> TimerDevice {
            self.device.CR1.write_whole_register(value);
            self
        }

        pub unsafe fn set_ccr4_register(self, value: u32) -> TimerDevice {
            self.device.CCR4.replace_whole_register(value);
            self
        }
        pub unsafe fn set_ccr3_register(self, value: u32) -> TimerDevice {
            self.device.CCR3.replace_whole_register(value);
            self
        }
        pub unsafe fn set_ccr1_register(self, value: u32) -> TimerDevice {
            self.device.CCR1.replace_whole_register(value);
            self
        }
        pub unsafe fn set_ccr2_register(self, value: u32) -> TimerDevice {
            self.device.CCR2.replace_whole_register(value);
            self
        }
        pub unsafe fn set_ccer_register(self, value: u32) -> TimerDevice {
            self.device.CCER.write_whole_register(value);
            self
        }
        pub unsafe fn set_egr_register(self, value: u32) -> TimerDevice {
            self.device.EGR.write_whole_register(value);
            self
        }
    }
}

pub mod stm32f3x {

    pub mod adresses {

        pub mod gpio {
            pub const GPIOA_BASE: u32 = 0x4800_0000;
            pub const GPIOB_BASE: u32 = 0x4800_0400;
            pub const GPIOC_BASE: u32 = 0x4800_0800;
            pub const GPIOE_BASE: u32 = 0x4800_1000;
        }
        pub const I2C1_BASEADRESS: u32 = 0x4000_5400;
        pub const TIM2_BASEADRESS: u32 = 0x4000_0000;
        pub const TIM3_BASEADRESS: u32 = 0x4000_0400;
        pub const TIM4_BASEADRESS: u32 = 0x4000_0800;
        pub const TIM6_BASEADRESS: u32 = 0x4000_1000;
        pub const TIM15_BASEADRESS: u32 = 0x4001_4000;
        pub const USART1_BASEADRESS: u32 = 0x4001_3800;

        pub mod dma {
            pub const DMA1: u32 = 0x4002_0000;
        }

        // manuel page 55
        pub const RCC: u32 = 0x4002_1000;
    }

    pub mod offsets {
        pub mod dma {
            pub const DMA_ISR: u32 = 0x00;
            pub const DMA_CCR: u32 = 0x08 + 0x0d20;
            pub const DMA_CNDTR: u32 = 0x0C + 0x0d20;
            pub const DMA_CPAR: u32 = 0x10 + 0x0d20;
            pub const DMA_CMAR: u32 = 0x14 + 0x0d20;
        }

        pub mod gpio {
            pub const GPIO_MODER: u32 = 0x00;
            pub const GPIO_OTYPER: u32 = 0x04;
            pub const GPIO_ODR: u32 = 0x14;
            pub const GPIO_AFRL: u32 = 0x20;
            pub const GPIO_AFRH: u32 = 0x24;
        }
        pub mod rcc {
            pub const RCC_APB1RSTR: u32 = 0x10;
            pub const RCC_AHBENR: u32 = 0x14;
            pub const RCC_APB2ENR: u32 = 0x18;
            pub const RCC_APB1ENR: u32 = 0x1C;
        }
        pub mod usart1 {
            pub const BRR: u32 = 0x0C;
            pub const ISR: u32 = 0x1C;
            pub const ICR: u32 = 0x20;
            pub const TDR: u32 = 0x28;
        }
        pub mod tim {
            pub const DIER: u32 = 0x0C;
            pub const SR: u32 = 0x10;
            pub const EGR: u32 = 0x14;
            pub const CNT: u32 = 0x24;
            pub const PSC: u32 = 0x28;
            pub const CCR1: u32 = 0x34;
        }
    }

    pub mod bitfields {
        pub mod gpio {
            pub const INPUT: u32 = 0b00;
            pub const GENERALPURPOSEOUTPUT: u32 = 0b01;
            pub const ALTERNATE: u32 = 0b10;
            pub const ANALOG: u32 = 0b11;
        }

        pub mod i2c {
            pub const PE: u32 = 0;
            pub const PRESC: u32 = 28;
            pub const SCLDEL: u32 = 20;
            pub const SDADEL: u32 = 16;
            pub const SCLH: u32 = 8;
            pub const SCLL: u32 = 0;
            pub const START: u32 = 13;
            pub const STOP: u32 = 14;
            pub const ADDR: u32 = 3;
            pub const TXE: u32 = 0;
            pub const RD_WRN: u32 = 10;
            pub const NBYTES: u32 = 16;
            pub const AUTOEND: u32 = 25;
            pub const RXNE: u32 = 2;
            pub const SADD_7_1: u32 = 1;
            pub const TXIS: u32 = 1;
            pub const TC: u32 = 6;
            // pub const PE: u32 = 0;
        }

        pub mod rcc {
            pub const SYSCFGEN: u32 = 1;
            pub const GPIOPAEN: u32 = 17;
            pub const GPIOPBEN: u32 = 18;
            pub const GPIOPCEN: u32 = 19;
            pub const GPIOPEEN: u32 = 21;
            pub const USART1EN: u32 = 14;
            pub const TIM2EN: u32 = 0;
            pub const TIM3EN: u32 = 1;
            pub const TIM4EN: u32 = 2;
            pub const TIM6EN: u32 = 4;
            pub const TIM15EN: u32 = 16;
            pub const I2C1EN: u32 = 21;
        }
        pub mod usart1 {
            pub const UE: u32 = 0;
            pub const RE: u32 = 2;
            pub const TE: u32 = 3;
            pub const RXNEIE: u32 = 5;
        }

        pub mod tim3 {
            pub mod cr1 {
                pub const ARPE: u32 = 7;
                pub const CEN: u32 = 0;
            }
            pub mod ccer {
                pub const CC1E: u32 = 0;
                pub const CC2E: u32 = 4;
                pub const CC3E: u32 = 8;
                pub const CC4E: u32 = 12;
                pub const CEN: u32 = 0;
            }
            pub mod ccmr1 {
                pub const OC2M: u32 = 12;
                pub const OC2PE: u32 = 11;
                pub const OC1M: u32 = 4;
                pub const OC1PE: u32 = 3;
            }
            pub mod ccmr2 {
                pub const OC4M: u32 = 12;
                pub const OC4PE: u32 = 11;
                pub const OC3M: u32 = 4;
                pub const OC3PE: u32 = 3;
            }
            pub const CEN: u32 = 0b1;
            pub const UG: u32 = 0b1;
            pub const UDIS: u32 = 0b10;
            pub const CC1IE: u32 = 0b10;
        }

        pub mod tim {
            pub const CEN: u32 = 0b1;
            pub const UG: u32 = 0b1;
            pub const UDIS: u32 = 0b10;
            pub const CC1IE: u32 = 0b10;
        }
    }
}

pub mod stm32f407ve {
    pub mod adresses {
        pub mod gpio {
            // page 65 ref. man
            pub const GPIOA_BASE: u32 = 0x4002_0000;
        }

        // manual page 55
        pub const RCC: u32 = 0x4002_3800;
        pub const USART1_BASEADRESS: u32 = 0x4001_1000;
    }

    pub mod bitfields {
        pub mod gpio {
            pub const INPUT: u32 = 0b00;
            pub const GENERALPURPOSEOUTPUT: u32 = 0b01;
            pub const ALTERNATE: u32 = 0b10;
            pub const ANALOG: u32 = 0b11;
        }
        pub mod rcc {
            pub const GPIOPAEN: u32 = 0;
        }
    }
}

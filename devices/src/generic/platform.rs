pub mod stm32f3x {
    pub mod adresses {
        pub mod gpio {
            pub const GPIOA_BASE: u32 = 0x4800_0000;
            pub const GPIOB_BASE: u32 = 0x4800_0400;
            pub const GPIOC_BASE: u32 = 0x4800_0800;
            pub const GPIOE_BASE: u32 = 0x4800_1000;
        }
        pub const TIM2_BASEADRESS: u32 = 0x4000_0000;
        pub const TIM3_BASEADRESS: u32 = 0x4000_0400;
        pub const USART1_BASEADRESS: u32 = 0x4001_3800;

        pub mod dma {
            pub const DMA1: u32 = 0x4002_0000;
        }

        // manuel page 55
        pub const RCC: u32 = 0x4002_1000;

    }

    pub mod offsets {
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
            pub const TDR: u32 = 0x28;
            pub const ISR: u32 = 0x1C;
            pub const BRR: u32 = 0x0C;
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
        pub mod rcc {
            pub const SYSCFGEN: u32 = 1;
            pub const IOPAEN: u32 = 17;
            pub const IOPEEN: u32 = 21;
            pub const USART1EN: u32 = 14;
        } 
        pub mod usart1 {
            
        }
        pub mod tim {
            pub const CEN: u32 = 0b1;
            pub const UG: u32 = 0b1;
            pub const UDIS: u32 = 0b10;
            pub const CC1IE: u32 = 0b10;
        }
    }
}
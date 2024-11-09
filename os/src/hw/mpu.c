//@leave it
//     volatile unsigned int *shcsr = (void *)0xE000ED24;
//     *shcsr |= (0x1 << 16) | (0x1 << 17) | (0x1 << 18);

//    *((unsigned int*) 0xE000ED98) = *((unsigned int*) 0xE000ED98) | 0x3; //
//    *((unsigned int*) 0xE000ED9C) = 0x20000200;//((unsigned int) address + 16) & ~16; // rbar

//     volatile unsigned int *mpu_rasr = (void *)0xE000EDA0;

//     *mpu_rasr = (0b000 << 24) | (0b000110 << 16) | (4 << 1) | 0x1;
//     volatile unsigned int *mpu_ctrl = (void *)0xE000ED94;
//     *mpu_ctrl = 0x5;
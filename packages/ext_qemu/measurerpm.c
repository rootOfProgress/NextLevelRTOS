#define TIM2_BASE 0x40000000
#define RCC 0x40021000

void start() {
    *(unsigned int*) TIM2_BASE = *(unsigned int*) TIM2_BASE | 1;
}

void stop() {
    *(unsigned int*) TIM2_BASE = *(unsigned int*) TIM2_BASE & !1;
}

void clear_udis() {
    *(unsigned int*) TIM2_BASE = *(unsigned int*) TIM2_BASE & !(1 << 1); 
}

void flush() {
    *(unsigned int*) (TIM2_BASE | 0x24) = *(unsigned int*) (TIM2_BASE | 0x24) & !(0xFFFF); 
}

unsigned int read_timer() {
    return (*(unsigned int*) (TIM2_BASE | 0x24) & ~(1 << 31));
}

void clear_uif() {
    *(unsigned int*) (TIM2_BASE | 0x10) = *(unsigned int*) (TIM2_BASE | 0x10) & !(0xF | 0b1 << 6); 
}

void set_ug() {
    *(unsigned int*) (TIM2_BASE | 0x14) = *(unsigned int*) (TIM2_BASE | 0x14) | 1; 
}

void enable_interrupt() {
    *(unsigned int*) (TIM2_BASE | 0xC) = (*(unsigned int*) (TIM2_BASE | 0xC)) | (1 << 1); 
}

void set_ccr(unsigned int ccr) {
    *(unsigned int*) (TIM2_BASE | 0x34) = ccr;
}


void set_prescaler(unsigned int psc) {
    *(unsigned int*) (TIM2_BASE | 0x28) = psc;
}

void reset_timer() {
    *(unsigned int*) 0x40021010 = *(unsigned int*) 0x40021010 | (1 << 1);
    *(unsigned int*) 0x40021010 = *(unsigned int*) 0x40021010 & !(1 << 1);
}

int __attribute((section(".main"))) main(void)
{   
    // void (*fn)() = (void (*)())0x080000dc;
    *(unsigned int*) (RCC | 0x1C) = *(unsigned int*) (RCC | 0x1C) | 1;
    reset_timer();
    set_prescaler(1000);
    set_ug();
    clear_uif();
    clear_udis();
    set_ccr(1000);
    unsigned int rpm = 0;
    unsigned int previous = 1;
    while(1){ 
        start();
            while (read_timer() < 0x1F40)
            {
                        unsigned int n = *((unsigned int*)0x48000010);
                        if (((n & 1)) < previous)
                        {
                            rpm++;
                            // fn();
                        }/* code */
                        previous = n & 1;

            }
        stop();
        rpm *= 60;
        flush();
        rpm = 0; 
        // __asm__("bkpt");
    };
    return 1;
}
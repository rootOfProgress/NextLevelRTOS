#define SV_YIELD_TASK __asm volatile ("mov r6, 2\n" \
                                  "svc 0\n")

#define SV_PRINT __asm volatile ("mov r6, 1\n" \
                                  "svc 0\n")
typedef struct transfer {
    void* start_adress;
    unsigned int length;
} TransferInfo_t;
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


__attribute__((used)) __attribute__((optimize("O0"))) void print_foo(volatile unsigned int* transfer_info)
{
  SV_PRINT;
}

typedef struct MemoryStatistic {
    unsigned int num_of_allocs;
    unsigned int num_of_deallocs;
    unsigned int num_of_fractial_allocs;
    unsigned int total_byte_alloced;
    unsigned int total_byte_used;
    unsigned int os_data_end;
    unsigned int free_useable;
} MemoryStatistic_t;

void send_number(unsigned int number, char converted[])
{   
    // unsigned int n = number;
    int cnt = 8;
    
    if (number == 0)
    {
        converted[0] = 0 + 0x30;
    }
    else
    {   
        while (number > 0)
        {   
            converted[cnt] = (number % 10) + 0x30;
            number /= 10;
            cnt--;
        }
    }
}


void __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{
    volatile TransferInfo_t t;
    char *s = "task up2\n\r";
    t.length = 9;
    t.start_adress = s;
    print_foo((unsigned int*) &t);

    *(unsigned int*) (RCC | 0x1C) = *(unsigned int*) (RCC | 0x1C) | 1;
    reset_timer();
    set_prescaler(1000);
    set_ug();
    clear_uif();
    clear_udis();
    set_ccr(1000);
    unsigned int rpm = 0;
    unsigned int previous = 1;

    // char s1[11] = {0,0,0,0,0,0,0,0,0,'\n','\r'};
    // s1[9] = '\n';
    // s1[10] = '\r';
    // send_number(160,s1);
    t.length = 7;
    char *msg = "task!\n\r";
    t.start_adress = msg;
    print_foo((unsigned int*) &t);

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
        print_foo((unsigned int*) &t);
        // SV_YIELD_TASK;
        // __asm__("bkpt");
    };
    // struct foo {
    //     unsigned int a;
    //     unsigned int b;
    // };

    // unsigned int start = 0x89ab;
    // unsigned int end = 0xcdef;
    // struct foo c = {.a=0x1111, .b=0x2222};

    // t.length = 2;
    // t.start_adress = &start;
    // print_foo((unsigned int*) &t);

    // t.length = sizeof(MemoryStatistic_t);
    // t.start_adress = (void*) 0x20000144; 
    // print_foo((unsigned int*) &t);

    // t.length = 2;
    // t.start_adress = &end;
    // print_foo((unsigned int*) &t);
}

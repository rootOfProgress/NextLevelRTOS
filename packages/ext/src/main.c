#define SV_YIELD_TASK __asm volatile ("mov r6, 2\n" \
                                  "svc 0\n")

#define SV_PRINT __asm volatile ("mov r6, 1\n" \
                                  "svc 0\n")
typedef struct transfer {
    void* start_adress;
    unsigned int length;
} TransferInfo_t;

#define BUFFERLENGTH 5
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

void num_to_char(unsigned int number, char converted[])
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

void init_buffer(char *buffer)
{
    for (unsigned int i = 0; i < BUFFERLENGTH; i++)
        *(buffer+i) = '0';
    buffer[BUFFERLENGTH - 1] = '\n';
    buffer[BUFFERLENGTH - 2] = '\r';
}

void init()
{
    *(unsigned int*) (RCC | 0x1C) = *(unsigned int*) (RCC | 0x1C) | 1;
    reset_timer();
    set_prescaler(1000);
    set_ug();
    clear_uif();
    clear_udis();
    set_ccr(1000);
}


void __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{
    volatile TransferInfo_t t;
    init();
    unsigned int rpm = 0;
    unsigned int previous = 1;

    t.length = BUFFERLENGTH;
    char buffer[BUFFERLENGTH];
    init_buffer(buffer);

    while(1){ 
        start();
            while (read_timer() < 0x1F40)
            {
                        unsigned int n = *((unsigned int*)0x48000010);
                        if (((n & 1)) < previous)
                        {
                            rpm++;
                        }
                        previous = n & 1;
            }
        stop();
        rpm *= 60;
        num_to_char(rpm, buffer);
        t.start_adress = &buffer;
        print_foo((unsigned int*) &t);
        flush();
        init_buffer(buffer);
        rpm = 0; 
    };
}

#define SV_YIELD_TASK __asm volatile ("mov r6, 2\n" \
                                  "svc 0\n")

#define SV_PRINT __asm volatile ("mov r6, 1\n" \
                                  "svc 0\n")
typedef struct transfer {
    void* start_adress;
    unsigned int length;
} TransferInfo_t;

#define READ_REGISTER(addr)     (*(volatile unsigned int *) (addr))
#define WRITE_REGISTER(addr, val) ((*(volatile unsigned int *) (addr)) = (unsigned int) (val))

#define BUFFERLENGTH 7
#define TIM2_BASE 0x40000000
#define RCC 0x40021000

typedef enum transfer_types {
    GENERIC = 0,
    MEM_ADDRESS,
    MEM_STAT,
    STATISTIC
} TransferType_t;


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

// wrong , not needed! would affect tim3!
void reset_timer() {
    // RCC_APB1RSTR
    *(unsigned int*) 0x40021010 = *(unsigned int*) 0x40021010 | (1 << 1);
    *(unsigned int*) 0x40021010 = *(unsigned int*) 0x40021010 & !(1 << 1);
}

__attribute__((used))  void uprint(volatile unsigned int* transfer_info __attribute__((unused)), volatile unsigned int type __attribute__((unused)))
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
    short cnt = BUFFERLENGTH - 1 - 2;
    while (number > 0)
    {   
        converted[cnt--] = (number % 10) + 0x30;
        number /= 10;
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
    WRITE_REGISTER((RCC | 0x1C), READ_REGISTER((RCC | 0x1C)) | 1);
    // reset_timer();
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
    // unsigned int rpm_print = 0;
    unsigned int previous = 1;

    t.length = BUFFERLENGTH;
    char buffer[BUFFERLENGTH];
    init_buffer(buffer);

    while(1){ 
        start();
            // tim counts with 8kHz
            // example:
            // every 125 usec one cycle
            // after 8000 (0x1f40) cycles 1 second passed
            while (read_timer() < 0x1F40)
            {
                unsigned int n = *((unsigned int*)0x48000010);
                if (((n & 1)) < previous)
                    rpm++;
                previous = n & 1;
            }
        stop();
        // 1 sec
        rpm *= 60;
        // rpm_print = rpm;
        if (rpm != 0)
            num_to_char(rpm, buffer);
        t.start_adress = &buffer;
        uprint((unsigned int*) &t, GENERIC);
        flush();
        init_buffer(buffer);
        rpm = 0; 
        SV_YIELD_TASK;
    };
}

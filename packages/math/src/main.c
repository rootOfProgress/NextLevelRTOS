#define SV_YIELD_TASK __asm volatile ("mov r6, 2\n" \
                                  "svc 0\n")

#define SV_PRINT __asm volatile ("mov r6, 1\n" \
                                  "svc 0\n")
typedef struct transfer {
    void* start_adress;
    unsigned int length;
} TransferInfo_t;

#include "os_api.h"

#define READ_REGISTER(addr)     (*(volatile unsigned int *) (addr))
#define WRITE_REGISTER(addr, val) ((*(volatile unsigned int *) (addr)) = (unsigned int) (val))

#define BUFFERLENGTH 7
#define TIM2_BASE 0x40000000
#define RCC 0x40021000
#include "math.h"
typedef enum transfer_types {
    GENERIC = 0,
    MEM_ADDRESS,
    MEM_STAT,
    STATISTIC
} TransferType_t;




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

unsigned int __errno;

void __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{
    // WRITE_REGISTER(0x48000014, READ_REGISTER(0x48000014) & ~1);
//
    WRITE_REGISTER(0x48000000, READ_REGISTER(0x48000000) | 0b01);
    WRITE_REGISTER(0x48000014, READ_REGISTER(0x48000014) & ~1);

    // BREAK;
    unsigned int* memory = allocate(8);
    // BREAK;
    char buffer[BUFFERLENGTH];
    init_buffer(buffer);
    WRITE_REGISTER(0x48000014, READ_REGISTER(0x48000014) | 1);
    // while(1){ 
        for (unsigned int i = 0; i < 100; i++)
        {
            float bar = cosf(i);
            float baz = powf(2,i);
            // float bar = 234.43 + (float) i;
        }
    WRITE_REGISTER(0x48000014, READ_REGISTER(0x48000014) & ~1);

    // };
}

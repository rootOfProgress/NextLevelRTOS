#define SV_YIELD_TASK __asm volatile ("mov r6, 2\n" \
                                  "svc 0\n")

#define SV_PRINT __asm volatile ("mov r6, 1\n" \
                                  "svc 0\n")
typedef struct transfer {
    void* start_adress;
    unsigned int length;
} TransferInfo_t;

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
    t.length = 7;
    char *msg = "task!\n\r";
    t.start_adress = msg;
    print_foo((unsigned int*) &t);

    while(1){
        // print_foo((unsigned int*) &t);
        SV_YIELD_TASK;
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

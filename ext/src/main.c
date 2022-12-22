#define SV_YIELD_TASK __asm volatile ("mov r6, 2\n" \
                                  "svc 0\n")

#define SV_PRINT __asm volatile ("mov r6, 1\n" \
                                  "svc 0\n")
typedef struct transfer {
    void* start_adress;
    unsigned int length;
} TransferInfo_t;

typedef struct lifetime_data
{
    unsigned int start; // 8 byte
    void* payload;
    unsigned int end; // 8 byte
} LifetimeData_t;


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


void __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{
    volatile TransferInfo_t t;
    struct foo {
        unsigned int a;
        unsigned int b;
    };

    unsigned int start = 0x89ab;
    unsigned int end = 0xcdef;
    struct foo c = {.a=0x1111, .b=0x2222};

    t.length = 2;
    t.start_adress = &start;
    print_foo((unsigned int*) &t);

    t.length = sizeof(MemoryStatistic_t);
    t.start_adress = (void*) 0x20000144; 
    print_foo((unsigned int*) &t);

    t.length = 2;
    t.start_adress = &end;
    print_foo((unsigned int*) &t);
            // SV_YIELD_TASK;
    // while (1)
    // {
        // for (unsigned int i = 0; i < 10; i++)
        // {
        //     char *s = "ABC\n\r";
        //     t.length = 5;
        //     t.start_adress = s; 
        //     print_foo((unsigned int*) &t);
        //     SV_YIELD_TASK;
        // }
    // }
}

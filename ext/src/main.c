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

void __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{
    volatile TransferInfo_t t;
    char *s = "ABC\n\r";
    t.length = 5;
    t.start_adress = s; 
    print_foo((unsigned int*) &t);

    // while (1)
    // {
    //     SV_YIELD_TASK;
    // }
}

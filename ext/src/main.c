#define SV_YIELD_TASK __asm volatile ("mov r6, 2\n" \
                                  "svc 0\n")

#define SV_PRINT __asm volatile ("mov r6, 1\n" \
                                  "svc 0\n")

void __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O2"))) main(void)
{
        SV_PRINT;
    while (1)
    {
        SV_YIELD_TASK;
        /* code */
    }
    
    //*((unsigned int*) 0x20002000) = 0xabcd1234;    
}

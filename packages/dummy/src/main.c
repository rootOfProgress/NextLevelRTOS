#define SV_YIELD_TASK __asm volatile ("mov r6, 2\n" \
                                  "svc 0\n")

#define SV_PRINT __asm volatile ("mov r6, 1\n" \
                                  "svc 0\n")

#include "os_api.h"

void __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{
    volatile unsigned int* memory_start = allocate(4);
    if (memory_start != 0)
        WRITE_REGISTER(0x20000000, READ_REGISTER(0x20000000) | 1 << 0);

    volatile unsigned int dealloc_result = deallocate((void*)memory_start);
    if (dealloc_result != 0)
        WRITE_REGISTER(0x20000000, READ_REGISTER(0x20000000) | 1 << 1);  

    volatile unsigned int* memory_second = allocate(4);
    if (memory_start == memory_second)
        WRITE_REGISTER(0x20000000, READ_REGISTER(0x20000000) | 1 << 2);
        
    volatile unsigned int* memory_8byte = allocate(8);
    deallocate(memory_8byte);
    volatile unsigned int* memory_4byte_one = allocate(4);

    if (memory_8byte == memory_4byte_one)
        WRITE_REGISTER(0x20000000, READ_REGISTER(0x20000000) | 1 << 3);    

    // fill gaps
    allocate(4);
    allocate(4);
    allocate(4);

    volatile unsigned int* memory_32byte = allocate(32);
    deallocate(memory_32byte);    

    volatile unsigned int* mem_1 = allocate(4);
    volatile unsigned int* mem_2 = allocate(4);
    volatile unsigned int* mem_3 = allocate(4);
    volatile unsigned int* mem_4 = allocate(16);
    volatile unsigned int* mem_5 = allocate(32);
    volatile unsigned int* mem_6 = allocate(4);


    if (memory_32byte == mem_1)
        WRITE_REGISTER(0x20000000, READ_REGISTER(0x20000000) | 1 << 4);  

    if ((memory_32byte + 1) == mem_2)
        WRITE_REGISTER(0x20000000, READ_REGISTER(0x20000000) | 1 << 5);  

    if ((memory_32byte + 2) == mem_3)
        WRITE_REGISTER(0x20000000, READ_REGISTER(0x20000000) | 1 << 6);  
    // BREAK;

    if ((memory_32byte + 3) == mem_4)
        WRITE_REGISTER(0x20000000, READ_REGISTER(0x20000000) | 1 << 7); 

    if ((memory_32byte + 8) == mem_5)
        WRITE_REGISTER(0x20000000, READ_REGISTER(0x20000000) | 1 << 8); 

    if ((memory_32byte + 7) == mem_6)
        WRITE_REGISTER(0x20000000, READ_REGISTER(0x20000000) | 1 << 9);

    volatile unsigned int* zero = allocate(88888888);
    if (zero == 0)
        WRITE_REGISTER(0x20000000, READ_REGISTER(0x20000000) | 1 << 10); 

    if (deallocate((unsigned int*) 0xab312f31) == 0)
        WRITE_REGISTER(0x20000000, READ_REGISTER(0x20000000) | 1 << 11); 
}

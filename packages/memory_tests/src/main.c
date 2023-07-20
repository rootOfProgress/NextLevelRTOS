#define SV_YIELD_TASK __asm volatile ("mov r6, 2\n" \
                                  "svc 0\n")

#define SV_PRINT __asm volatile ("mov r6, 1\n" \
                                  "svc 0\n")

#include "os_api.h"
#include "rcc.h"
#include "gpio.h"
#include "uart.h"
#include "tim2_5.h"
#include "parameters.h"

typedef struct MeasurementResults {
    char Subtest002_000_memset_byte; 
    char Subtest002_001_alloc_zero_or_less; 
    char Subtest002_002_alloc_negative; 
    char reserved[29]; 
} MeasurementResults_t;

typedef struct DummyStruct {
    unsigned int a0;
    unsigned int a1;
    char a2[4];
} DummyStruct_t;

char Subtest002_000_memset_byte(void)
{
    DummyStruct_t dt;
    dt.a0 = 0x12345678;
    dt.a1 = 0xaaaaaaaa;
    dt.a2[0] = 'c';

    memset_byte((void*) &dt, sizeof(DummyStruct_t), 0);
    
    if (dt.a0 == 0 || dt.a1 == 0 || dt.a2[0] == 0 || dt.a2[3] == 0)
    {
        return 1;
    }
    return 0;
}

char Subtest002_001_alloc_zero_or_less(void)
{
    void* zero = (void*) allocate(0);
    asm("bkpt");
    void* negative = (void*) allocate(-1);
    if (zero || negative)
        return 0;
    return 1;
}

int __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{
    MeasurementResults_t measurements;
    measurements.Subtest002_000_memset_byte = 0;

    measurements.Subtest002_000_memset_byte = Subtest002_000_memset_byte();
    measurements.Subtest002_001_alloc_zero_or_less = Subtest002_001_alloc_zero_or_less();
    // DummyStruct_t dt;
    // dt.a0 = 0x12345678;
    // dt.a1 = 0xaaaaaaaa;
    // dt.a2[0] = 'c';

    // memset_byte((void*) &dt, sizeof(DummyStruct_t), 0);
    
    // if (dt.a0 == 0 || dt.a1 == 0 || dt.a2[0] == 0 || dt.a2[3] == 0)
    // {
    //     measurements.Subtest002_000_memset_byte = 1;
    // }
    // else
    // {
    //     print(&measurements, sizeof(int));
    //     return;
    // }
       

    print((char*) &measurements, sizeof(MeasurementResults_t));
    return 0;
}

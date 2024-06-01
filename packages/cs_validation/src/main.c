#define SV_YIELD_TASK __asm volatile ("mov r6, 2\n" \
                                  "svc 0\n")

#define SV_PRINT __asm volatile ("mov r6, 1\n" \
                                  "svc 0\n")

#include "os_api.h"
#include "rcc.h"
#include "uart.h"
#include "tim2_5.h"
#include "addresses.h"

typedef struct MeasurementResults 
{
    unsigned int results[32]; 
} MeasurementResults_t;

typedef struct RegisterValues 
{
  unsigned short a;
  unsigned short b;
  unsigned short c;
  unsigned short d;
  unsigned short e;
  unsigned short f;
  unsigned int g;
  unsigned int h;
  unsigned int i;
} RegisterValues_t;
RegisterValues_t reg;

int __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{
    reg.a = 0xAAAA;
    reg.b = 0xBBBB;
    reg.c = 0xCCCC;
    reg.d = 0xDDDD;
    reg.e = 0xEEEE;
    reg.f = 0xFFFF;
    reg.g = 0x11111111;
    reg.h = 0x33333333;
    reg.i = 0x22222222;

    MeasurementResults_t measurements;
    measurements.results[0] = 1;

    for (unsigned int j = 0; j < 10; j++)
    {
        sleep(100);
        if (!(reg.a == 0xAAAA || reg.b == 0xBBBB || reg.c == 0xCCCC || 
              reg.d == 0xDDDD || reg.e == 0xEEEE || reg.f == 0xFFFF|| 
              reg.g == 0x11111111 || reg.h == 0x33333333 || reg.i == 0x22222222)) 
        {
            measurements.results[0] = 0;
            break;
        }
    }
        
    // asm("bkpt");
    print((char*) &measurements.results, sizeof(int));
    return 0;
}

#define SV_YIELD_TASK __asm volatile ("mov r6, 2\n" \
                                  "svc 0\n")

#define SV_PRINT __asm volatile ("mov r6, 1\n" \
                                  "svc 0\n")
#define TIM3_BASE 0x40000400

void set_ccr_1(unsigned int ccr) {
    *(unsigned int*) (TIM3_BASE | 0x34) = ccr;
}

void set_ccr_2(unsigned int ccr) {
    *(unsigned int*) (TIM3_BASE | 0x38) = ccr;
}

void set_ccr_3(unsigned int ccr) {
    *(unsigned int*) (TIM3_BASE | 0x3C) = ccr;
}

void set_ccr_4(unsigned int ccr) {
    *(unsigned int*) (TIM3_BASE | 0x40) = ccr;
}



__attribute__((used)) __attribute__((optimize("O0"))) void print_foo(volatile unsigned int* transfer_info)
{
  SV_PRINT;
}

void __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O0"))) main(void)
{
    set_ccr_1(16);
    set_ccr_2(16);
    set_ccr_3(16);
    set_ccr_4(16);
}

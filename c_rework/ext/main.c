#define svc(code) asm volatile ("mov r6, %[immediate]\n" \
                                " SVC \#0"::[immediate] "I" (code) \
                                 : "r6")

void __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O2"))) main(void)
{
    volatile typedef int func(void);
    volatile func* f = (func*)0x080001a8;


    // svc(1);
    f(); 
}

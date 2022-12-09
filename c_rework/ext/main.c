#define svc(code) asm volatile ("mov r6, %[immediate]\n" \
                                " SVC \#0"::[immediate] "I" (code) \
                                 : "r6")

void __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O2"))) main(void)
{
    typedef int func(void);
    func* f = (func*)0x080001a8;
    f();
}

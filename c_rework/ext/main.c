#define svc(code) asm volatile ("mov r6, %[immediate]\n" \
                                " SVC \#0"::[immediate] "I" (code) \
                                 : "r6")

void __attribute((section(".main"))) __attribute__((__noipa__))  __attribute__((optimize("O2"))) main(void)
{
    *((unsigned int*) 0x20002000) = 0x12345678;

    while(1){ 
        svc(1);
    };
    // return 1;
}

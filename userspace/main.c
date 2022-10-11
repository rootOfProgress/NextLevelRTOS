enum trap_type {
    FOO = 0,
    BAR
};


void os_call(enum trap_type t)
{
    switch ((unsigned int) t)
    {
    case 0:
        __asm__("mov r0, 0xA");
        break;
    case 1:
        __asm__("mov r0, 0xB");
    default:
        break;
    }
    __asm__("svc 0");
}
int fibonacci(int n) {
    switch (n)
    {
        case 0: 
        { 
            return 1; 
        }
        case 1: 
        { 
            return 1;
        }
        default:
        {
            return fibonacci(n - 1) + fibonacci(n - 2);
        }
    }
}


int __attribute((section(".main"))) main(void)
{   
    int a = 0x3FE;
    __asm__("mov r0, 0xAB");
    __asm__("mov r1, 0xBC");
    __asm__("mov r2, 0xCD");
    while(1){    };
    return 1;
}

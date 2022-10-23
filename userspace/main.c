
int read(int target);
void bar(int value, int target);

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
    // __asm__("svc 0");
}


int __attribute((section(".main"))) main(void)
{   
    int a = 0x3FE;
    __asm__("mov r0, 0xAB");
    __asm__("mov r1, 0xBC");
    bar(0x4, 0x48000014);
    int b = read(0xE000E010);
    int c = read(0x48000014);
    os_call(FOO);
    while(1){ 
        // __asm__("bkpt");
    };
    return 1;
}

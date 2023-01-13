#define INTERRUPT __attribute__((interrupt))
#define NULL ( (void *) 0)

#define READ_REGISTER(addr)     (*(volatile unsigned int *) (addr))
#define WRITE_REGISTER(addr, val) ((*(volatile unsigned int *) (addr)) = (unsigned int) (val))

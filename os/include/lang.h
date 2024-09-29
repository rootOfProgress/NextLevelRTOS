#define INTERRUPT __attribute__((interrupt))
#define ISR_INTERNAL __attribute__ ((interrupt ("IRQ")))
#define UNUSED __attribute__((unused))
#define USED __attribute__((used))
#define NO_OPT __attribute__((optimize("O0")))
#define NULL ( (void *) 0)

#define READ_REGISTER(addr)     (*(volatile unsigned int *) (addr))
#define WRITE_REGISTER(addr, val) ((*(volatile unsigned int *) (addr)) = (unsigned int) (val))

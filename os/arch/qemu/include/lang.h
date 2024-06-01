#ifndef LANG_H
#define LANG_H

#define INTERRUPT __attribute__((interrupt))
#define ISR __attribute__ ((interrupt ("IRQ")))
#define UNUSED __attribute__((unused))
#define USED __attribute__((used))
#define NO_OPT __attribute__((optimize("O0")))
#define NULL ( (void *) 0)

#define READ_REGISTER(addr)     (*(volatile unsigned int *) (addr))
#define WRITE_REGISTER(addr, val) ((*(volatile unsigned int *) (addr)) = (unsigned int) (val))
#define SET_BIT(register_addr, bit_position) ((*(volatile unsigned int *) (register_addr)) = ((*(volatile unsigned int *) (register_addr)) | bit_position))
#define CLEAR_BIT(register_addr, bit_position) ((*(volatile unsigned int *) (register_addr)) = ((*(volatile unsigned int *) (register_addr)) & ~(bit_position)))
#define FLUSH_REGISTER(addr) ((*(volatile unsigned int *) (addr)) = READ_REGISTER(addr) & ~(0xFFFFFFFF))

#endif
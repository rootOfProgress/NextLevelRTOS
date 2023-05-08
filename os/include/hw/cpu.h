#ifndef CPU_H
#define CPU_H

#include "../lang.h"

typedef enum {
    Icsr = 0xE000ED04
} Scb_t;

typedef struct CpuRegister {
    unsigned int r4; // 0x00
    unsigned int r5;
    unsigned int r6; // 0x08
    unsigned int r7;
    unsigned int r8; // 0x10
    unsigned int r9;
    unsigned int r10; // 0x18
    unsigned int r11;
    unsigned int r0; // 0x20
    unsigned int r1;
    unsigned int r2; // 0x28
    unsigned int r3;
    unsigned int r12; // 0x30
    unsigned int lr;
    unsigned int pc; // 0x38
    unsigned int psr;
} CpuRegister_t; 

typedef enum {
    PendSVClear = 27,
    PendSVSet = 28,
} Icsr_t;

enum { 
    CPU_NVIC_ISER0 = 0xE000E100, 
    CPU_NVIC_ISER1 = 0xE000E104, 
    CPU_NVIC_ISER2 = 0xE000E108, 
    CPU_NVIC_ISER3 = 0xE000E10C, 
    CPU_NVIC_ISER4 = 0xE000E110, 
    CPU_NVIC_ISER5 = 0xE000E114, 
    CPU_NVIC_ISER6 = 0xE000E118, 
    CPU_NVIC_ISER7 = 0xE000E11C, 
};

enum { 
    CPU_SCB_CCR = 0xE000ED14,
};

enum { 
    CPU_SCB_SHCSR = 0xE000ED24,
};


enum { STK = 0xE000E010 };

#define AIRCR 0xE000ED0C

#define CTRL 0x00
#define LOAD 0x04
#define VAL 0x08
#define CALIB 0x0C

typedef struct Systick {
    unsigned int ctrl;
    unsigned int load;
    unsigned int val;
    unsigned int calib;
} SystickRegisters_t;

static inline __attribute__((always_inline)) void enable_systick(void)
{
    SystickRegisters_t* stk = (SystickRegisters_t*) STK;
    WRITE_REGISTER(&stk->ctrl, READ_REGISTER(&stk->ctrl) | 1);
}

static inline __attribute__((always_inline)) void soft_reset(void)
{
    __asm("dsb");
    WRITE_REGISTER(AIRCR, 0x05FA0001);
    __asm("dsb");

    // reboot went wrong
   __builtin_unreachable();
}

static inline __attribute__((always_inline)) void set_pendsv(void)
{
    *(unsigned int*) Icsr = *(unsigned int*) Icsr | 1 << PendSVSet;
}

static inline __attribute__((always_inline)) void enable_irq(void)
{
    __asm ("CPSIE I");
}

static inline __attribute__((always_inline)) void disable_irq(void)
{
    __asm ("CPSID I");
}

static inline __attribute__((always_inline)) void sleep(void)
{
    __asm ("wfi");
}

static inline void disable_systick(void)
{
    SystickRegisters_t* stk = (SystickRegisters_t*) STK;
    WRITE_REGISTER(&stk->ctrl, READ_REGISTER(&stk->ctrl) & ~1);
}

void init_systick(unsigned int);
void sleep(void);

#endif
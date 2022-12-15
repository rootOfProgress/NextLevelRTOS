#ifndef CPU_H
#define CPU_H

// #define ICSR 0xE000ED04 

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

#endif
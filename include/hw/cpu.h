#ifndef CPU_H
#define CPU_H

// #define ICSR 0xE000ED04 

typedef enum {
    Icsr = 0xE000ED04
} Scb_t;

typedef struct CpuRegister {
    unsigned int r4;
    unsigned int r5;
    unsigned int r6;
    unsigned int r7;
    unsigned int r8;
    unsigned int r9;
    unsigned int r10;
    unsigned int r11;
    unsigned int r0;
    unsigned int r1;
    unsigned int r2;
    unsigned int r3;
    unsigned int r12;
    unsigned int lr;
    unsigned int pc;
    unsigned int psr;
} CpuRegister_t; 

typedef enum {
    PendSVClear = 27,
    PendSVSet = 28,
} Icsr_t;

#endif
#ifndef TCB_H
#define TCB_H

typedef struct Tcb {
    unsigned int pid;
    unsigned int* sp;
    unsigned int* memory_lower_bound;
    unsigned int* memory_upper_bound;
} Tcb_t;

Tcb_t* new_tcb(unsigned int,unsigned int,unsigned int,unsigned int);

#endif
#ifndef TCB_H
#define TCB_H

typedef enum {
    READY = 0,
    WAITING,
    FINISHED
} TaskState_t;

typedef struct Tcb {
    unsigned int pid;
    unsigned int sp;
    unsigned int memory_lower_bound;
    unsigned int memory_upper_bound;
    unsigned int code_section;
    TaskState_t task_state;
} Tcb_t;

Tcb_t* new_tcb(unsigned int,unsigned int,unsigned int,unsigned int);

#endif
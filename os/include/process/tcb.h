#ifndef TCB_H
#define TCB_H

typedef enum {
    READY = 0,
    WAITING,
    FINISHED,
    DEAD,
    INVALID
} TaskState_t;

typedef struct InterruptStatistic {
    unsigned long voluntary_interrupts;
    unsigned long forced_interrupts;
} InterruptStatistic_t;

typedef struct Tcb {
    unsigned int pid;
    unsigned int sp;
    unsigned int memory_lower_bound;
    unsigned int memory_upper_bound;
    unsigned int code_section;
    InterruptStatistic_t irq_statistic[DEBUG ? 1 : 0];
    TaskState_t task_state;
} Tcb_t;

Tcb_t* new_tcb(unsigned int,unsigned int,unsigned int,unsigned int);

#endif
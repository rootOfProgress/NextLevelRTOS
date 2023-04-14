#ifndef TCB_H
#define TCB_H

typedef enum {
    READY = 0,
    WAITING,
    FINISHED,
    DEAD,
    INVALID
} TaskState_t;

typedef union TaskInfo {
    struct {
        unsigned int pid : 4, state : 3, is_external : 1, stack_size : 16, : 8;
    } task_info;
    unsigned int raw;
} TaskInfo_t;

typedef union TaskLifetimeData {
    struct {
        unsigned int voluntary_interrupts;
        unsigned int forced_interrupts;
        unsigned int cpu_time;
    } lifetime;
    unsigned int raw[3];
} TaskLifetimeData_t;

enum { IsNoExternalTask = 0, IsExternalTask = 1 };

typedef struct Tcb {
    TaskInfo_t general;
    unsigned int sp;
    unsigned int memory_lower_bound;
    unsigned int code_section;
    TaskLifetimeData_t lifetime_info[(DEBUG == 1 || DEBUG == 2)? 1 : 0];
} Tcb_t;

#endif
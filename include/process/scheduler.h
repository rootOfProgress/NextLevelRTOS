#ifndef SCHEDULER_H
#define SCHEDULER_H
#include "data/queue.h"
#include "process/tcb.h"
#include "lang.h"
#include "exception.h"

extern void __trap(TrapType_t, unsigned int);

extern Queue_t* task_queue;
extern Node_t* currently_running;
extern void (*switch_task)();

void next_task(void);
void policy_round_robin(void);
void remove_current_task(void);
void do_selfcheck_scheduler(void);
void init_scheduler(void);
void insert_scheduled_task(Tcb_t*);
void remove_scheduled_task(void);
void run_scheduler(void);
void load_task(void);

#endif
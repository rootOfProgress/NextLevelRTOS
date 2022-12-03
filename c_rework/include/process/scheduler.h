#ifndef SCHEDULER_H
#define SCHEDULER_H
#include "data/queue.h"
#include "process/tcb.h"
#include "lang.h"
#include "exception.h"

extern void __trap(TrapType_t, unsigned int);

extern Queue_t* task_queue;
extern Node_t* currently_running;
// static void (*switch_task)() = NULL;

void next_task(void);

// @todo: inline?
void policy_round_robin(void);
// static void inline policy_fifo(void)
// {   
//     Tcb_t *tcb_of_next = NULL;
//     // return head only
//     if ((tcb_of_next = next_task()) == NULL)
//     {
//         // activate idle task which forces sleep.
//     }
//     return;
// }
void do_selfcheck_scheduler(void);
void init_scheduler(void);
void insert_scheduled_task(Tcb_t*);
void remove_scheduled_task(void);
void run_scheduler(void);

#endif
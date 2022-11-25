#ifndef SCHEDULER_H
#define SCHEDULER_H
#include "data/queue.h"
#include "process/tcb.h"
#include "lang.h"
#include "exception.h"

extern void __trap(TrapType_t, unsigned int);

static Queue_t* task_queue = NULL;
static Node_t* currently_running = NULL;
static void (*switch_task)() = NULL;

Tcb_t* next_task(void);

static void inline policy_round_robin(void)
{   
    Tcb_t *tcb_of_next = NULL;
    if ((tcb_of_next = next_task()) == NULL)
    {
        // activate idle task which forces sleep.
    }
}

static void inline policy_fifo(void)
{   
    Tcb_t *tcb_of_next = NULL;
    // return head only
    if ((tcb_of_next = next_task()) == NULL)
    {
        // activate idle task which forces sleep.
    }
}

void init_scheduler(void);
void insert_scheduled_task(Tcb_t*);
void remove_scheduled_task(void);
void run_scheduler(void);

#endif
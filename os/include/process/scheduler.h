#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "data/queue.h"
#include "process/tcb.h"
#include "lang.h"
#include "memory.h"
#include "exception.h"

#define TASK_TRANSFER_HANDLER 0
#define TASK_STATISTIC 1
#define TASK_RPM 3

typedef struct proc_stats {
    unsigned int num_of_hardfaults;
} ProcessStats_t;

extern ProcessStats_t* process_stats;
extern Queue_t* running_tasks;
extern Node_t* currently_running;
extern Queue_t* waiting_tasks;
extern void (*switch_task)();

static inline __attribute__((always_inline)) void wakeup_pid(unsigned int pid)
{
    Node_t *q = get_head_element(waiting_tasks);
    if (!q)
        return;

    for (unsigned int i = 0; i < waiting_tasks->size; i++)
    {
        if (((Tcb_t*)q->data)->pid == pid)
        {
            ((Tcb_t*)q->data)->task_state = READY;

            isolate_node(waiting_tasks,q);
            move_node(running_tasks,q);
            return;
        }
        q = q->next;
    }
}

void next_task(void);
void policy_round_robin(void);
void remove_current_task(void);
void init_scheduler(void);
void insert_scheduled_task(Tcb_t*);
void remove_scheduled_task(void);
void run_scheduler(void);
void wakeup_pid(unsigned int);
void block_current_task(void);
void invalidate_current_task(void);
void unblock_task(unsigned int);
void reboot(void);
void finish_task(void);
void search_invalidate_tasks(void);
void clean_up_task(Tcb_t*, Node_t*);

#endif
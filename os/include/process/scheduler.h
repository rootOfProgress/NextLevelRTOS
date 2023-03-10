#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "data/queue.h"
#include "process/tcb.h"
#include "hw/cpu.h"
#include "lang.h"
#include "memory.h"
#include "exception.h"

typedef struct proc_stats {
    unsigned int num_of_hardfaults;
    unsigned int num_of_systick_interrupts;
    unsigned int num_of_svcalls;
    unsigned int num_of_pendsv;
} ProcessStats_t;

typedef struct KernelPids {
    char idle_task;
    char transfer_handler;
    char statistic_manager;
    char generic_printer;
} KernelPids_t;

extern ProcessStats_t* process_stats;
extern KernelPids_t kernel_pids;

extern Node_t* currently_running;
extern Node_t* task_to_preserve;

extern Queue_t* running_tasks;
extern Queue_t* waiting_tasks;

extern void (*switch_task)();


static inline __attribute__((always_inline)) void block_current_task(void)
{
    ((Tcb_t*) (currently_running->data))->general.task_info.state = WAITING;

    // On context switch, task->next is loaded. Jump back one task here ensures that next task is 
    // in fact the successor of the one that gets blocked right now.
    Node_t* q = currently_running->prev;

    // SV_YIELD_TASK preserves the state of the currently running task. Currently running will be pointing to
    // its predecor, so without repointing the Node_t a not-running task would be preserved.
    task_to_preserve = currently_running;

    // Delete currently_running from active queue und shift it into waiting queue.
    isolate_node(running_tasks, currently_running);
    move_node(waiting_tasks, currently_running);

    currently_running = q;
    SV_YIELD_TASK;
}

static inline __attribute__((always_inline)) void wakeup_pid(unsigned int pid)
{
    disable_systick();
    Node_t *q = get_head_element(waiting_tasks);
    if (!q)
        return;

    for (unsigned int i = 0; i < waiting_tasks->size; i++)
    {
        if (((Tcb_t*)q->data)->general.task_info.pid == pid)
        {
            isolate_node(waiting_tasks,q);
            move_node(running_tasks,q);
            ((Tcb_t*)q->data)->general.task_info.state = READY;
            return;
        }
        q = q->next;
    }
    enable_systick();
}

void next_task(void);
void policy_round_robin(void);
void remove_current_task(void);
void init_scheduler(void);
void insert_scheduled_task(Tcb_t*);
void remove_scheduled_task(void);
void run_scheduler(void);
void invalidate_current_task(void);
void unblock_task(unsigned int);
void reboot(void);
void finish_task(void);
void search_invalidate_tasks(void);
void clean_up_task(Tcb_t*, Node_t*);

#endif
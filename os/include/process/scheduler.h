#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "data/queue.h"
#include "process/tcb.h"
#include "hw/cpu.h"
#include "lang.h"
#include "memory.h"
#include "exception.h"
#include "panic.h"

typedef struct proc_stats {
    unsigned int num_of_hardfaults : 4, num_of_finished_tasks : 4, clean_up_requests : 4, : 20;
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

    currently_running = running_tasks->size > 0 ? q : NULL;
    SV_YIELD_TASK;
}

static inline __attribute__((always_inline)) Node_t* wakeup_pid(unsigned int pid)
{
    Node_t *q = get_head_element(waiting_tasks);
    if (!q)
        return NULL;

    for (unsigned int i = 0; i < waiting_tasks->size; i++)
    {
        if (((Tcb_t*)q->data)->general.task_info.pid == pid)
        {
            isolate_node(waiting_tasks,q);
            move_node(running_tasks,q);
            ((Tcb_t*)q->data)->general.task_info.state = READY;
            return q;
        }
        q = q->next;
    }
    return NULL;
}

static inline __attribute__((always_inline)) void switch_task(void)
{
    if (!currently_running)
    {
        currently_running = wakeup_pid(kernel_pids.idle_task);
        if (!currently_running)
            invoke_panic(SCHEDULER_NOT_INITIALIZED);
        task_to_preserve = currently_running;
    }

    for (unsigned int j = 0; j < running_tasks->size; j++)
    {
        currently_running = currently_running->next;
        Tcb_t* n = (Tcb_t*) currently_running->data;
        if (n->general.task_info.state == READY)
            task_to_preserve = currently_running;
    }
}

static inline __attribute__((always_inline)) void restore_psp()
{
      __asm volatile (
        "mrs r2, psp\n"
        "ldmfd r2!, {r4-r11}\n"
        "msr psp, r2\n"
      );
}

#endif
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "data/queue.h"
#include "process/tcb.h"
#include "hw/cpu.h"
#include "lang.h"
#include "memory.h"
#include "exception.h"
#include "panic.h"
#include "dma.h"
#include "tim2_5.h"

typedef struct proc_stats {
    unsigned int num_of_hardfaults : 4, num_of_finished_tasks : 4, clean_up_requests : 4, : 20;
    unsigned int num_of_systick_interrupts;
    unsigned int num_of_svcalls;
    unsigned int num_of_pendsv;
    PanicTypes_t panic_state;
} ProcessStats_t;

typedef struct KernelPids {
    char idle_task;
    char transfer_handler;
    char statistic_manager;
    char generic_printer;
} KernelPids_t;

enum { ResolutionForSysLogging = 5 };       // <! Task runtimes are measured in 5 usec ticks 
enum { ResolutionForSleepFunction = 1000 }; // <! Task sleep unit is 1 msec
enum { TimerForSysLogging = 3 };            // <! Task runtimes are tracked by timer 5
enum { TimerForTaskSleep = 3 };             // <! Task runtimes are tracked by timer 5

typedef struct TaskSleepRequest
{
    unsigned int pid_of_sleeping_task;
} TaskSleepRequest_t;

extern TaskSleepRequest_t task_sleep_request;

extern ProcessStats_t process_stats;
extern KernelPids_t kernel_pids;

extern Node_t* currently_running;
extern Node_t* task_to_preserve;

extern Queue_t* running_tasks;
extern Queue_t* waiting_tasks;

void policy_round_robin(void);
void remove_current_task(void);
int init_scheduler(void);
int insert_scheduled_task(Tcb_t*);
int run_scheduler(void);
void invalidate_current_task(void);
void reboot(void);
void finish_task(void);
void search_invalidate_tasks(void);
void clean_up_task(Tcb_t*, Node_t*);
void task_sleep(unsigned int);

// workaround
void force_pid0_into_running(void);


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
        {
            force_pid0_into_running();
        }
        
        task_to_preserve = currently_running;
        return;
    }

    if (dma_interrupt_action & DmaTransferedExternalTask)
        wakeup_pid(kernel_pids.idle_task);

    for (unsigned int j = 0; j < running_tasks->size; j++)
    {
        currently_running = currently_running->next;
        Tcb_t* n = (Tcb_t*) currently_running->data;
        if (n->general.task_info.state == READY)
        {
            task_to_preserve = currently_running;
            return;
        }
    }

    currently_running, task_to_preserve = wakeup_pid(kernel_pids.idle_task);

    // @todo Workaround if systick=1
    // Not yet resolved: Pid0 is not ready and not yet moved to waiting queue
    if (!currently_running)
    {
        force_pid0_into_running();
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
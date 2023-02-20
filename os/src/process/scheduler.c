#include "process/scheduler.h"
#include "hw/cpu.h"
#include "memory.h"
#include "panic.h"

Queue_t* running_tasks = NULL;
Queue_t* waiting_tasks = NULL;
Node_t* currently_running = NULL;
Node_t* task_to_preserve = NULL;
ProcessStats_t* process_stats = NULL;

void (*switch_task)();

void init_scheduler(void)
{
    running_tasks = new_queue();
    waiting_tasks = new_queue();
    process_stats = (ProcessStats_t*) allocate(sizeof(ProcessStats_t));
    memset_byte((void*) process_stats, sizeof(ProcessStats_t*), 0);
    switch_task = policy_round_robin;
}

void insert_scheduled_task(Tcb_t* tcb)
{
    enqueue_element(running_tasks, (Tcb_t*) tcb);
}

void reboot(void)
{
    soft_reset();
}

void __attribute__ ((hot)) policy_round_robin(void)
{   
    if (!currently_running)
    {
        currently_running = (Node_t*) get_head_element(running_tasks);
        return;
    }

    // if (((Tcb_t*) currently_running->data)->pid == 0)
    //     ((Tcb_t*) currently_running->data)->task_state = WAITING;

    for (unsigned int j = 0; j < running_tasks->size; j++)
    {
        currently_running = currently_running->next;
        Tcb_t* n = (Tcb_t*) currently_running->data;
        if (n->task_state == READY)
        {
            task_to_preserve = currently_running;
            return;
        }
    }
    while (1)
    {
        // no task found, do something useful here
    }
    
    // no runnable task found, wakeup pid0
    // currently_running = (Node_t*) get_head_element(waiting_tasks);
    // ((Tcb_t*) currently_running->data)->task_state = READY;
}

void invalidate_current_task(void)
{
    ((Tcb_t*) (currently_running->data))->task_state = DEAD;
}

void block_current_task(void)
{
    disable_systick();
    ((Tcb_t*) (currently_running->data))->task_state = WAITING;

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
    enable_systick();
    SV_YIELD_TASK;
}

void run_scheduler(void)
{
    if (!running_tasks)
        invoke_panic(SCHEDULER_NOT_INITIALIZED);

    currently_running = (Node_t*) get_head_element(running_tasks);
    task_to_preserve = currently_running;
    SV_EXEC_PSP_TASK;
}


void __attribute__ ((hot)) PendSV(void)
{
    if (DEBUG)
        process_stats->num_of_pendsv++;
    __asm volatile ("mrs %0, psp" : "=r"(((Tcb_t*) task_to_preserve->data)->sp));
    switch_task();
    __asm volatile ("mov r2, %[next_sp]":: [next_sp] "r" (((Tcb_t*) currently_running->data)->sp));
    __asm volatile (
      "ldmfd r2!, {r4-r11}\n"
      "msr psp, r2\n"
    );
}

void clean_up_task(Tcb_t* t, Node_t* obsolete_node)
{
    if (!deallocate((unsigned int*) t->memory_lower_bound))
        invoke_panic(MEMORY_DEALLOC_FAILED);

    if (t->code_section != 0)
    {
        if (!deallocate((unsigned int*) t->code_section))
            invoke_panic(MEMORY_DEALLOC_FAILED);
    }
    
    if (!deallocate((unsigned int*) t))
        invoke_panic(MEMORY_DEALLOC_FAILED);

    Node_t* old_element = dequeue_element(running_tasks, obsolete_node);

    if (!deallocate((unsigned int*) old_element))
        invoke_panic(MEMORY_DEALLOC_FAILED);      
}

void search_invalidate_tasks(void) 
{
    Node_t* q = (Node_t*) currently_running;
    for (unsigned int j = 0; j < running_tasks->size; j++)
    {
        Tcb_t* t = (Tcb_t*) q->data;
        if (t->task_state == FINISHED || t->task_state == INVALID)
            clean_up_task(t, q);
        q = q->next;
    }
}

void finish_task(void)
{
    Tcb_t* n = (Tcb_t*) currently_running->data;
    n->task_state = FINISHED;

    switch_task();

    SV_EXEC_PSP_TASK;
}


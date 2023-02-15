#include "process/scheduler.h"
#include "hw/cpu.h"
#include "memory.h"
#include "panic.h"

Queue_t* task_queue = NULL;
Queue_t* low_priority_tasks = NULL;
Queue_t* waiting_tasks = NULL;
Node_t* currently_running = NULL;
ProcessStats_t* process_stats = NULL;

void (*switch_task)();

void init_scheduler(void)
{
    task_queue = new_queue();
    low_priority_tasks = new_queue();
    waiting_tasks = new_queue();
    process_stats = (ProcessStats_t*) allocate(sizeof(ProcessStats_t));
    memset_byte((void*) process_stats, sizeof(ProcessStats_t*), 0);
    switch_task = policy_round_robin;
}

void insert_scheduled_task(Tcb_t* tcb)
{
    enqueue_element(task_queue, (Tcb_t*) tcb);
    mstat.total_scheduled_tasks++;
}

void reboot(void)
{
    soft_reset();
}

void __attribute__ ((hot)) policy_round_robin(void)
{   
    if (!currently_running)
    {
        currently_running = (Node_t*) get_head_element(task_queue);
        return;
    }

    if (((Tcb_t*) currently_running->data)->pid == 0)
        ((Tcb_t*) currently_running->data)->task_state = WAITING;

    for (unsigned int j = 0; j < task_queue->size; j++)
    {
        currently_running = currently_running->next;
        Tcb_t* n = (Tcb_t*) currently_running->data;
        if (n->task_state == READY)
            return;
    }
    // no runnable task found, wakeup pid0
    currently_running = (Node_t*) get_head_element(task_queue);
    ((Tcb_t*) currently_running->data)->task_state = READY;
}

void invalidate_current_task(void)
{
    ((Tcb_t*) (currently_running->data))->task_state = DEAD;
}

void block_current_task(void)
{
    ((Tcb_t*) (currently_running->data))->task_state = WAITING;
    mstat.waiting_tasks++;
    SV_YIELD_TASK;
}

void run_scheduler(void)
{
    if (!task_queue)
        invoke_panic(SCHEDULER_NOT_INITIALIZED);

    currently_running = (Node_t*) get_head_element(task_queue);
    SV_EXEC_PSP_TASK;
}


void __attribute__ ((hot)) PendSV(void)
{
    __asm volatile ("mrs %0, psp" : "=r"(((Tcb_t*) currently_running->data)->sp));
    switch_task();
    __asm volatile ("MOV R2, %[input_i]":: [input_i] "r" (((Tcb_t*) currently_running->data)->sp));
    __asm volatile (
      "LDMFD r2!, {r4-r11}\n"
      "MSR PSP, r2\n"
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

    Node_t* old_element = dequeue_element(task_queue, obsolete_node);

    if (!deallocate((unsigned int*) old_element))
        invoke_panic(MEMORY_DEALLOC_FAILED);      
}

void search_invalidate_tasks(void) 
{
    Node_t* q = (Node_t*) currently_running;
    for (unsigned int j = 0; j < task_queue->size; j++)
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
    mstat.total_scheduled_tasks--;

    switch_task();

    SV_EXEC_PSP_TASK;
}


#include "process/scheduler.h"
#include "hw/cpu.h"
#include "memory.h"
#include "panic.h"

Queue_t* task_queue = NULL;
Queue_t* low_priority_tasks = NULL;
Node_t* currently_running = NULL;
void (*switch_task)();

void init_scheduler(void)
{
    task_queue = new_queue();
    low_priority_tasks = new_queue();
    switch_task = policy_round_robin;
}

void insert_scheduled_task(Tcb_t* tcb)
{
    enqueue_element(task_queue, (Tcb_t*) tcb);
}


void __attribute__ ((hot)) policy_round_robin(void)
{   
    if (!currently_running)
    {
        currently_running = (Node_t*) get_head_element(task_queue);
        return;
    }

    for (unsigned int j = 0; j < task_queue->size; j++)
    {
        currently_running = currently_running->next;
        Tcb_t* n = (Tcb_t*) currently_running->data;
        if (n->task_state == READY)
            return;
    }
    // move sleeping task
}

void load_task(void)
{
    currently_running = (Node_t*) get_head_element(task_queue);
}


void run_scheduler(void)
{
    // get task
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


void remove_scheduled_task(void)
{
    Tcb_t* t = (Tcb_t*) currently_running->data;
    deallocate((unsigned int*) t->memory_lower_bound);
    if (t->code_section != 0)
        deallocate((unsigned int*) t->code_section);
    deallocate((unsigned int*) t);

    dequeue_element(task_queue, currently_running);
    switch_task();

    SV_EXEC_PSP_TASK;
}
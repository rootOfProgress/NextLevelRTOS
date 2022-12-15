#include "process/scheduler.h"
#include "hw/cpu.h"
#include "memory.h"

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


void policy_round_robin(void)
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
        return;

    currently_running = (Node_t*) get_head_element(task_queue);
    svc(EXEC_PSP_TASK);
}


void PendSV(void)
{
    // *(unsigned int*) Icsr = *(unsigned int*) Icsr | 1 << PendSVClear;
    __asm__("mrs %0, psp" : "=r"(((Tcb_t*) currently_running->data)->sp));
    switch_task();
    __asm__ volatile ("MOV R2, %[input_i]":: [input_i] "r" (((Tcb_t*) currently_running->data)->sp));
    __asm (
      "LDMFD r2!, {r4-r11}\n"
      "MSR PSP, r2\n"
    );
}


void remove_scheduled_task(void)
{
    Tcb_t* t = (Tcb_t*) currently_running->data;
    deallocate(t->memory_lower_bound);
    if (t->code_section != 0)
        deallocate(t->code_section);
    deallocate(t);

    dequeue_element(task_queue, currently_running);
    switch_task();

    svc(EXEC_PSP_TASK);
}
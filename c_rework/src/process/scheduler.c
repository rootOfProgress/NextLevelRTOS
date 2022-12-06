#include "process/scheduler.h"
#include "hw/cpu.h"

Queue_t* task_queue = NULL;
Node_t* currently_running = NULL;
void (*switch_task)();

void init_scheduler(void)
{
    task_queue = new_queue();
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
    *(unsigned int*) Icsr = *(unsigned int*) Icsr | 1 << PendSVClear;
    unsigned int old_sp;
    unsigned int new_sp;
    
    __asm__("mrs r2, psp");
    __asm__("mov %0, r2" : "=r"(old_sp));
    ((Tcb_t*) currently_running->data)->sp = old_sp;
    switch_task();
    __asm__("mov lr, 0xFFFFFFFD");
    new_sp = ((Tcb_t*) currently_running->data)->sp;

    __asm__ volatile ("MOV R2, %[input_i]":: [input_i] "r" (new_sp));
    __asm (
      "TST lr, #4\n"
      "ITT NE\n"
      "LDMFDNE r2!, {r4-r11}\n"
      "MSRNE PSP, r2\n"
    ) ;
}

void remove_scheduled_task(void)
{
    dequeue_element(task_queue, currently_running);
    currently_running = NULL;
    currently_running = (Node_t*) get_head_element(task_queue);

    svc(EXEC_PSP_TASK);
}
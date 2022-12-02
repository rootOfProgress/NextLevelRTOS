#include "process/scheduler.h"
#include "hw/cpu.h"

Queue_t* task_queue = NULL;
Node_t* currently_running = NULL;
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
    // Tcb_t *tcb_of_next = NULL;
    // if ((currently_running->data = next_task()) == NULL)
    // {
    //     // activate idle task which forces sleep.
    // }
    return;
}

void run_scheduler(void)
{
    // get task
    if (!task_queue)
        return;

    currently_running = (Node_t*) get_head_element(task_queue);
    Tcb_t* t = currently_running->data;

    __trap(RUN_THREAD_MODE, (unsigned int) t->sp);
}


void PendSV(void)
{
    *(unsigned int*) Icsr = *(unsigned int*) Icsr | 1 << PendSVClear;
    // policy_round_robin();
    unsigned int sp = 0;

    __asm__("mrs r2,psp\n");
    __asm__("mov %0, r2" : "=r"(sp));
    ((Tcb_t*)currently_running->data)->sp = sp;
    currently_running = currently_running->next;

    // switch_task();


    unsigned int sp1 = ((Tcb_t*)currently_running->data)->sp;
    __asm__("mov r0, %0" : "=r"(sp1));
    __asm__("ldmfd r0!, {r4-r11}");
    __asm__("msr psp, r0");
    // __asm__("mov lr, \#0xfffffffd");
    // __asm__("mrs r0, msp");
    // __asm__("sub r0, \#-4*4");
    // __asm__("msr msp, r0");
    // __asm__("bx lr");
    
}

void remove_scheduled_task(void)
{
    dequeue_element(task_queue, currently_running);

    // Node_t* q = (Node_t*) get_head_element(task_queue);
    // for (unsigned int j = 0; j < task_queue->size; j++)
    // {
    //     Tcb_t* tcb = (Tcb_t*) q->data;
    //     if (tcb->pid == pid_of_currently_running)
    //     {
    //         dequeue_element(task_queue, q);
    //     }
    //     q = q->next;
    // }
}

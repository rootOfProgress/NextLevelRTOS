#include "process/scheduler.h"
#include "hw/cpu.h"

Queue_t* task_queue = NULL;
Node_t* currently_running = NULL;

void do_selfcheck_scheduler(void)
{
    
}

void init_scheduler(void)
{
    task_queue = new_queue();
    // switch_task = policy_round_robin;
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
    // Tcb_t* t = currently_running->data;
    // if ((currently_running->data = next_task()) == NULL)
    // {
    //     // activate idle task which forces sleep.
    // }
    svc(0);
    // __trap(RUN_THREAD_MODE, (unsigned int) t->sp);
}


void PendSV(void)
{
    *(unsigned int*) Icsr = *(unsigned int*) Icsr | 1 << PendSVClear;
    // policy_round_robin();

    // switch_task();
    unsigned int sp1 = (unsigned int) ((Tcb_t*)currently_running->data)->sp;
    // if ((currently_running->data = next_task()) == NULL)
    // {
    //     // activate idle task which forces sleep.
    // }
__asm__ volatile ("MOV R0, %[input_i]"
    :  
    : [input_i] "r" (sp1)
      );


    // __asm__ volatile("mov r0, %[input]" : [input] "r"(sp1) : "r0"\);
    __asm__ volatile("ldmfd r0!, {r4-r11}");
    __asm__ volatile("msr psp, r0");
    __asm__ volatile("mov lr, \#0xfffffffd");
    __asm__ volatile("bx lr");
    
    // __asm__("mrs r0, msp");
    // __asm__("sub r0, #-5*4");
    // __asm__("msr msp, r0");
    // __asm__("pop {lr}");
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

void next_task(void)
{
    policy_round_robin();
    while (1)//for (unsigned int j = 0; j < task_queue->size; j++)
    {
        policy_round_robin();
        currently_running = currently_running->next;
        Tcb_t* n = (Tcb_t*) currently_running->data;
        if (n->task_state == READY)
            return;
    }
    return NULL;
}


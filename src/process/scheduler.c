#include "process/scheduler.h"
#include "process/tcb.h"
#include "data/queue.h"

void init_scheduler(void)
{
    task_queue = new_queue();
    switch_task = policy_round_robin;
}

void insert_scheduled_task(Tcb_t* tcb)
{
    enqueue_element(task_queue, (Tcb_t*) tcb);
}

void run_scheduler(void)
{
    // get task
    if (!task_queue)
        return;

    Node_t* task_0 = get_head_element(task_queue);
    Tcb_t* t = task_0->data;
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

Tcb_t* next_task(void)
{
    for (unsigned int j = 0; j < task_queue->size; j++)
    {
        currently_running = currently_running->next;
        Tcb_t* n = (Tcb_t*) currently_running->data;
        if (n->task_state == READY)
            return currently_running->data;
    }
    return NULL;
}


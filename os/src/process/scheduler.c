#include "process/scheduler.h"
#include "hw/cpu.h"
#include "memory.h"
#include "panic.h"

Queue_t* running_tasks = NULL;
Queue_t* waiting_tasks = NULL;
Node_t* currently_running = NULL;
Node_t* task_to_preserve = NULL;

ProcessStats_t process_stats;
KernelPids_t kernel_pids;

void init_scheduler(void)
{
    running_tasks = new_queue();
    waiting_tasks = new_queue();
    
    memset_byte((void*) &process_stats, sizeof(ProcessStats_t), 0);
    memset_byte((void*) &kernel_pids, sizeof(KernelPids_t), -1);
}

void insert_scheduled_task(Tcb_t* tcb)
{
    switch (tcb->general.task_info.state)
    {
    case READY:
        enqueue_element(running_tasks, (Tcb_t*) tcb);
        break;
    case WAITING:
        enqueue_element(waiting_tasks, (Tcb_t*) tcb);
        break;
    default:
        break;
    }
}

void reboot(void)
{
    soft_reset();
}

void invalidate_current_task(void)
{
    ((Tcb_t*) (currently_running->data))->general.task_info.state = DEAD;
}

void run_scheduler(void)
{
    if (running_tasks->size == 0)
        if (!(wakeup_pid(kernel_pids.idle_task)))
            invoke_panic(SCHEDULER_NOT_INITIALIZED);

    currently_running = (Node_t*) get_head_element(running_tasks);
    task_to_preserve = currently_running;
    SV_EXEC_PSP_TASK;
}

void __attribute__ ((hot)) PendSV(void)
{
    if (DEBUG)
        process_stats.num_of_pendsv++;
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
        if (t->general.task_info.state == FINISHED || t->general.task_info.state == INVALID)
        {
            clean_up_task(t, q);
            process_stats.clean_up_requests--;
        }
        q = q->next;
    }
}

void finish_task(void)
{
    Tcb_t* n = (Tcb_t*) currently_running->data;
    n->general.task_info.state = FINISHED;

    if (DEBUG)
        process_stats.num_of_finished_tasks++;
    process_stats.clean_up_requests++;
    set_pendsv();
}


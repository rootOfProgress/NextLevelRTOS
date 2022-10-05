#include "../../include/process/scheduler.h"
int *addr = 0x20001000;

void run_init_process()
{
    TCB *t = create_task(123,456);
    while(1);
}

TCB* create_task(uint32_t stack_pointer, short pid)
{
    TCB *t = (TCB*) addr;
    t->pid = pid;
    t->stackpointer_adress = stack_pointer;
    // *addr = (TCB){pid, stack_pointer};
    // tcb.pid = pid;
    // tcb.stackpointer_adress = stack_pointer;
    return t;
}
#include "memory.h"
#include "hw/cpu.h"
#include "process/tcb.h"
#include "process/scheduler.h"


void create_task(void (*task_function)())
{
    unsigned int* address = allocate(sizeof(CpuRegister_t) + 512);
    // @todo
    CpuRegister_t* cpu_register =  (CpuRegister_t*) ((unsigned int) address + (unsigned int) 512 - sizeof(CpuRegister_t));

    if (!cpu_register)
    {
        // error
    }

    // CpuRegister_t* cpu_register = (CpuRegister_t*) (process_memory + sizeof(CpuRegister_t) + 512 - sizeof(CpuRegister_t));
    cpu_register->r4 = 0x4;
    cpu_register->r5 = 0x5;
    cpu_register->r6 = 0x6;
    cpu_register->r7 = 0x7;
    cpu_register->r8 = 0x8;
    cpu_register->r9 = 0x9;
    cpu_register->r10 = 0xA;
    cpu_register->r11 = 0xB;
    cpu_register->r0 = 0x0;
    cpu_register->r1 = 0x1;
    cpu_register->r2 = 0x2;
    cpu_register->r3 = 0x3;
    cpu_register->psr = 0x21000000;
    // todo
    cpu_register->pc = (unsigned int) task_function;
    cpu_register->lr = (unsigned int) remove_scheduled_task;

    // todo: useful values here

    // unsigned int *tcb_memory = allocate(sizeof(Tcb_t));
    Tcb_t *tcb = (Tcb_t*) allocate(sizeof(Tcb_t));
    tcb->pid = task_queue->size;
    tcb->sp = &cpu_register->r4;
    tcb->memory_lower_bound = address;
    tcb->memory_upper_bound = (address + 512 / 4);
    tcb->task_state = READY;

    insert_scheduled_task(tcb);
}
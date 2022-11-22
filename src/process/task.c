#include "memory.h"
#include "hw/cpu.h"
#include "process/tcb.h"
#include "process/scheduler.h"


void create_task(void (*task_function)())
{
    MemoryResult_t *process_memory = allocate(sizeof(CpuRegister_t) + 512);

    if (!process_memory)
    {
        // error
    }

    CpuRegister_t* cpu_register = (CpuRegister_t*) (process_memory->end_address - sizeof(CpuRegister_t));
    cpu_register->r0 = 0xAB;
    cpu_register->psr = 0x21000000;
    cpu_register->r4 = 0x66;
    cpu_register->r11 = 0xBC;
    // todo
    cpu_register->pc = (unsigned int*) task_function;
    cpu_register->lr = 0x0;

    // todo: useful values here
    cpu_register->psr = 0x21000000;

    Tcb_t *tcb = allocate(sizeof(Tcb_t));
    tcb->pid = task_queue->size;
    tcb->sp = &cpu_register->r4;
    tcb->memory_lower_bound = process_memory->start_address;
    tcb->memory_upper_bound = process_memory->end_address;

    insert(task_queue, (void*) tcb);

}
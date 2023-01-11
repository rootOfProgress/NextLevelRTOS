#include "memory.h"
#include "hw/cpu.h"
#include "process/tcb.h"
#include "process/scheduler.h"
#include "process/task.h"
#include "panic.h"

#define BUFFER 2048

CpuRegister_t* prepare_cpu_register(unsigned int address, unsigned int buffer_size, void (*task_function)())
{
    // @todo
    CpuRegister_t* cpu_register =  (CpuRegister_t*) ((unsigned int) address + (unsigned int) buffer_size - (unsigned int) sizeof(CpuRegister_t));

    if (!cpu_register)
        invoke_panic(OUT_OF_MEMORY);

    memset_byte((void*) cpu_register, sizeof(CpuRegister_t), 0);

    cpu_register->psr = THUMB_STATE;
    // todo
    cpu_register->pc = (unsigned int) task_function;
    cpu_register->lr = (unsigned int) remove_scheduled_task;
    return cpu_register;
}


void create_task(void (*task_function)(), unsigned int ram_location)
{

    unsigned int address = (unsigned int) allocate(sizeof(CpuRegister_t) + BUFFER);
    if (address == 0)
        invoke_panic(OUT_OF_MEMORY);

    CpuRegister_t *cpu_register = prepare_cpu_register(address, BUFFER, task_function);

    Tcb_t *tcb = (Tcb_t*) allocate(sizeof(Tcb_t));
    if (!tcb)
        invoke_panic(OUT_OF_MEMORY);

    tcb->pid = task_queue->size;
    tcb->sp = (unsigned int) &cpu_register->r4;
    tcb->memory_lower_bound = (unsigned int)address;
    tcb->memory_upper_bound = ((unsigned int)address + BUFFER);
    tcb->code_section = ram_location;
    tcb->task_state = READY;
    init_process_allocator((unsigned int*) tcb->memory_lower_bound);
//@leave it
//     volatile unsigned int *shcsr = (void *)0xE000ED24;
//     *shcsr |= (0x1 << 16) | (0x1 << 17) | (0x1 << 18);

//    *((unsigned int*) 0xE000ED98) = *((unsigned int*) 0xE000ED98) | 0x3; // 
//    *((unsigned int*) 0xE000ED9C) = 0x20000200;//((unsigned int) address + 16) & ~16; // rbar

//     volatile unsigned int *mpu_rasr = (void *)0xE000EDA0;

//     *mpu_rasr = (0b000 << 24) | (0b000110 << 16) | (4 << 1) | 0x1;
//     volatile unsigned int *mpu_ctrl = (void *)0xE000ED94;
//     *mpu_ctrl = 0x5;
    insert_scheduled_task((Tcb_t*) tcb);
}
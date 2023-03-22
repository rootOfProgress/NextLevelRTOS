#include "memory.h"
#include "hw/cpu.h"
#include "process/tcb.h"
#include "process/scheduler.h"
#include "process/task.h"
#include "panic.h"

#define STACK_SIZE 1536

unsigned int pid_of_transferhandler;
unsigned int pid_of_mstat;
unsigned int pid_of_foo;

CpuRegister_t* prepare_cpu_register(unsigned int address, unsigned int buffer_size, void (*task_function)())
{
    // @todo
    CpuRegister_t* cpu_register =  (CpuRegister_t*) ((unsigned int) address + (unsigned int) buffer_size - (unsigned int) sizeof(CpuRegister_t));

    if (!cpu_register)
        invoke_panic(OUT_OF_MEMORY);

    memset_byte((void*) cpu_register, sizeof(CpuRegister_t), 0);
    memset_byte((void*) address, STACK_SIZE - sizeof(CpuRegister_t), 0xA5);

    cpu_register->psr = THUMB_STATE;
    cpu_register->pc = (unsigned int) task_function;
    cpu_register->lr = (unsigned int) finish_task;
    return cpu_register;
}


unsigned int create_task(void (*task_function)(), unsigned int ram_location)
{

    unsigned int address = (unsigned int) allocate(sizeof(CpuRegister_t) + STACK_SIZE);
    if (!address)
        invoke_panic(OUT_OF_MEMORY);

    CpuRegister_t *cpu_register = prepare_cpu_register(address, STACK_SIZE, task_function);

    Tcb_t *tcb = (Tcb_t*) allocate(sizeof(Tcb_t));

    if (!tcb)
        invoke_panic(OUT_OF_MEMORY);

    // memset_byte((void*) &tcb, sizeof(Tcb_t), 0); 


    tcb->general.task_info.pid = running_tasks->size + waiting_tasks->size;
    tcb->general.task_info.state = READY;
    tcb->general.task_info.stack_size = STACK_SIZE;

    if (ram_location)
        tcb->general.task_info.is_external = IsExternalTask;

    tcb->sp = (unsigned int) &cpu_register->r4;
    tcb->memory_lower_bound = (unsigned int)address;
    tcb->code_section = ram_location;
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
    return tcb->general.task_info.pid;
}
#include "memory.h"
#include "hw/cpu.h"
#include "process/tcb.h"
#include "process/scheduler.h"
#include "process/task.h"
#include "panic.h"
#include "data/list.h"

// @todo
#define STACK_SIZE 1536

unsigned int pid_of_transferhandler;
unsigned int pid_of_mstat;
unsigned int pid_of_foo;

CpuRegister_t* prepare_cpu_register(unsigned int address, unsigned int buffer_size, void (*task_function)())
{
    CpuRegister_t* cpu_register =  (CpuRegister_t*) ((unsigned int) address + (unsigned int) buffer_size - (unsigned int) sizeof(CpuRegister_t));

    if (!cpu_register)
    {
        set_panic(OUT_OF_MEMORY);
        return NULL;
    }

    memset_byte((void*) cpu_register, sizeof(CpuRegister_t), 0);
    memset_byte((void*) address, STACK_SIZE - sizeof(CpuRegister_t), 0xA5);

    cpu_register->psr = THUMB_STATE;
    cpu_register->pc = (unsigned int) task_function;
    cpu_register->lr = (unsigned int) finish_task;
    return cpu_register;
}


int create_task(void (*task_function)(), unsigned int ram_location)
{
    unsigned int task_stack_start_address = (unsigned int) allocate(sizeof(CpuRegister_t) + STACK_SIZE);
    
    if (!task_stack_start_address)
    {
        set_panic(OUT_OF_MEMORY);
        return -1;
    }

    CpuRegister_t *cpu_register = prepare_cpu_register(task_stack_start_address, STACK_SIZE, task_function);
    
    if (!cpu_register)
        return -1;

    Tcb_t *tcb = (Tcb_t*) allocate(sizeof(Tcb_t));

    if (!tcb)
    {
        set_panic(OUT_OF_MEMORY);
        return -1;
    }
    else
    {
        memset_byte((void*) tcb, sizeof(Tcb_t), 0);
    }

    tcb->general.task_info.pid = running_tasks->size + waiting_tasks->size;
    tcb->general.task_info.state =  !tcb->general.task_info.pid ? WAITING : READY;
    tcb->general.task_info.stack_size = STACK_SIZE;
    tcb->stacksection_lower_bound = task_stack_start_address;

    if (ram_location != 0)
    {
        tcb->general.task_info.is_external = (char) IsExternalTask;
        tcb->codesection_lower_bound = ram_location;
        tcb->codesection_upper_bound = (ram_location + tInfo.task_size);
    }
    else 
    {
        tcb->general.task_info.is_external = (char) 0;
        tcb->codesection_lower_bound = -1;
        tcb->codesection_upper_bound = -1;
    }

    tcb->sp = (unsigned int) &cpu_register->r4;    
    tcb->code_section = ram_location;
    tcb->join_pid = -1;

    if (!currently_running)
    {
        tcb->parent_task = NULL;
    }
    else
    {
        tcb->parent_task = currently_running->data;
    }
    
    if (currently_running)
    {
        single_list_push(currently_running->data->child_tasks, (void*) &tcb);
    }
    
    tcb->child_tasks = new_list();
    
    if (DEBUG)
    {
        tcb->lifetime_info->lifetime.voluntary_interrupts = 0;
        tcb->lifetime_info->lifetime.forced_interrupts = 0;
        tcb->lifetime_info->lifetime.cpu_time = 0;
    }
//@leave it
//     volatile unsigned int *shcsr = (void *)0xE000ED24;
//     *shcsr |= (0x1 << 16) | (0x1 << 17) | (0x1 << 18);

//    *((unsigned int*) 0xE000ED98) = *((unsigned int*) 0xE000ED98) | 0x3; // 
//    *((unsigned int*) 0xE000ED9C) = 0x20000200;//((unsigned int) address + 16) & ~16; // rbar

//     volatile unsigned int *mpu_rasr = (void *)0xE000EDA0;

//     *mpu_rasr = (0b000 << 24) | (0b000110 << 16) | (4 << 1) | 0x1;
//     volatile unsigned int *mpu_ctrl = (void *)0xE000ED94;
//     *mpu_ctrl = 0x5;
    if (insert_scheduled_task((Tcb_t*) tcb) == -1)
    {
        return -1;
    }
    return tcb->general.task_info.pid;
}
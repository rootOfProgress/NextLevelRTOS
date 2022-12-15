#include "memory.h"
#include "hw/cpu.h"
#include "process/tcb.h"
#include "process/scheduler.h"
#include "process/task.h"

#define BUFFER 256

#ifdef SELF_CHECK
void do_selfcheck_task(void)
{
    unsigned int test_a = 1;
    unsigned int test_b = 1;
    unsigned int test_c = 1;
    unsigned int test_d = 1;
    unsigned int test_e = 1;
    unsigned int test_f = 1;
    unsigned int test_g = 1;
    unsigned int test_h = 1;
    unsigned int test_i = 1;
    unsigned int test_j = 1;
    unsigned int test_k = 1;
    unsigned int buffer_size = 512;

    unsigned int address = (unsigned int) allocate(sizeof(CpuRegister_t) + buffer_size);


    // test_a-: locations          
    CpuRegister_t *cpu_register = prepare_cpu_register(address, buffer_size, &do_selfcheck_task);
    if (&cpu_register->r4 != (buffer_size + address - sizeof(CpuRegister_t)))
        test_a = 0;
    if ((&cpu_register->r5 - &cpu_register->r4) != 1)
        test_b = 0;
    if (&cpu_register->r0 != (&cpu_register->r4 + 8))
        test_c = 0;
    if ((&cpu_register->r0 - &cpu_register->r11) != 1)
        test_d = 0;
    if ((&cpu_register->r0 + 7) != &cpu_register->psr)
        test_e = 0;
    if (cpu_register->pc != (unsigned int) &do_selfcheck_task)
        test_f = 0;
    if ((address + buffer_size + sizeof(CpuRegister_t)))
        test_g = 0;
    int p = (unsigned int) &cpu_register->psr;
    int end = (unsigned int) address + buffer_size + sizeof(CpuRegister_t);

    int diff = (buffer_size + address + sizeof(CpuRegister_t)) - (unsigned int) &cpu_register->r0;
}
#endif

CpuRegister_t* prepare_cpu_register(unsigned int address, unsigned int buffer_size, void (*task_function)())
{
    // @todo
    CpuRegister_t* cpu_register =  (CpuRegister_t*) ((unsigned int) address + (unsigned int) buffer_size - (unsigned int) sizeof(CpuRegister_t));

    if (!cpu_register)
    {
        // error
    }

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
    return cpu_register;
}


void create_task(void (*task_function)(), unsigned int ram_location)
{

    unsigned int address = (unsigned int) allocate(sizeof(CpuRegister_t) + BUFFER);
    if (address == 0)
    {
        while (1)
        {
            // @todo err handling
        }
        
    }
    CpuRegister_t *cpu_register = prepare_cpu_register(address, BUFFER, task_function);

    Tcb_t *tcb = (Tcb_t*) allocate(sizeof(Tcb_t));
    tcb->pid = task_queue->size;
    tcb->sp = (unsigned int) &cpu_register->r4;
    tcb->memory_lower_bound = (unsigned int)address;
    tcb->memory_upper_bound = ((unsigned int)address + BUFFER);
    tcb->code_section = ram_location;
    tcb->task_state = READY;
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
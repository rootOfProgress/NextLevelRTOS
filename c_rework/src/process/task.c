#include "memory.h"
#include "hw/cpu.h"
#include "process/tcb.h"
#include "process/scheduler.h"
#include "process/task.h"

void do_selfcheck_cpureg(void)
{

}

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

CpuRegister_t* prepare_cpu_register(unsigned int address, unsigned int buffer_size, void (*task_function)())
{
    // @todo
    CpuRegister_t* cpu_register =  (CpuRegister_t*) ((unsigned int) address + (unsigned int) buffer_size - (unsigned int) sizeof(CpuRegister_t));

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
    return cpu_register;
}


void create_task(void (*task_function)())
{
    unsigned int address = (unsigned int) allocate(sizeof(CpuRegister_t) + 512);
    if (address == 0)
    {
        while (1)
        {
            /* code */
        }
        
    }
    CpuRegister_t *cpu_register = prepare_cpu_register(address, 512, task_function);

    // todo: useful values here

    // unsigned int *tcb_memory = allocate(sizeof(Tcb_t));
    Tcb_t *tcb = (Tcb_t*) allocate(sizeof(Tcb_t));
    tcb->pid = task_queue->size;
    tcb->sp = &cpu_register->r4;
    tcb->memory_lower_bound = (unsigned int)address;
    tcb->memory_upper_bound = ((unsigned int)address + 512);
    tcb->task_state = READY;

    insert_scheduled_task((Tcb_t*) tcb);
}
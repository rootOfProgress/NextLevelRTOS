#ifndef TASK_H
#define TASK_H
#include "hw/cpu.h"

void create_task(void (*)());
void do_selfcheck_task(void);
CpuRegister_t* prepare_cpu_register(unsigned int, unsigned int, void (*)());

#endif
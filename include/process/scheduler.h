#ifndef SCHEDULER_H
#define SCHEDULER_H
#include "data/queue.h"
#include "lang.h"

static Queue_t* task_queue = NULL;

void run_init_process();

#endif
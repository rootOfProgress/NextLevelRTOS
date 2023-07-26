#ifndef EXTERNALS_H
#define EXTERNALS_H

#define NUM_OF_EXTERNAL_FUNCTIONS 1

extern void template2(void);

static void (*func_ptr[NUM_OF_EXTERNAL_FUNCTIONS])() = {template2,};

#endif
#ifndef EXTERNALS_H
#define EXTERNALS_H

#define NUM_OF_EXTERNAL_FUNCTIONS 2

extern void template1(void);
extern void template2(void);

static void (*func_ptr[NUM_OF_EXTERNAL_FUNCTIONS])() = {template1,template2,};

#endif
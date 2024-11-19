#ifndef GLOBALS_H
#define GLOBALS_H

enum 
{
  MaximumNumberOfAppendableFunctions = 6
};

enum
{
  readTimerFunctionPtr = 0,
  crcResetFunctionPtr = 1,
  crcLoadFunctionPtr = 2,
  crcReadFunctionPtr = 3,
  allocateFunctionPtr = 4,
  deallocateFunctionPtr = 5,
};

typedef union 
{
  unsigned int* (*funcWithArg)(unsigned int);
  unsigned int (*funcWithArgAndPtr)(unsigned int*);
  unsigned int (*funcNoArg)(void);
} FunctionPointer;

void append_os_core_function(FunctionPointer function, unsigned int functionIndex);

extern FunctionPointer osCoreFunctions[6];
#endif
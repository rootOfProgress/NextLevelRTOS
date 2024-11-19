#include "globals.h"

FunctionPointer osCoreFunctions[6] = {  };
static unsigned int current_ptr_index = 0;

void append_os_core_function(FunctionPointer function, unsigned int functionIndex)
{
  if (functionIndex > (MaximumNumberOfAppendableFunctions - 1))
  {
    return;
  }
  // osCoreFunctions[current_ptr_index++] = function;
  osCoreFunctions[functionIndex] = function;
}

#include "globals.h"

unsigned int (*osCoreFunctions[6])() = {  };
static unsigned int current_ptr_index = 0;

void append_os_core_function(unsigned int (*function)())
{
  osCoreFunctions[current_ptr_index++] = function;
}

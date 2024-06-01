#include "panic.h"
#include "process/scheduler.h"

PanicTypes_t panic_state;

void invoke_panic(PanicTypes_t panic_type)
{
  process_stats.panic_state = panic_type;
}
#include "runtime.h"
#include "process/scheduler.h"
#include "hw/cpu.h"
#include "memory.h"

void __attribute__((__noipa__)) __attribute__((optimize("O0"))) idle_runner(void)
{
    while (1)
    {
        if (process_stats.clean_up_requests)
            search_invalidate_tasks();
        // enable_systick();
        sleep();
        // disable_systick();
        block_current_task();
    }
}

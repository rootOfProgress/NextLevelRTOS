#include "runtime.h"
#include "process/scheduler.h"
#include "hw/cpu.h"
#include "memory.h"
#include "dma.h"
#include "process/task.h"

void __attribute__((__noipa__)) __attribute__((optimize("O0"))) idle_runner(void)
{
    while (1)
    {
        if (process_stats.clean_up_requests)
            search_invalidate_tasks();

        if (dma_interrupt_action & DmaTransferedExternalTask)
        {
            create_task((void (*)()) tInfo.start_adress, tInfo.task_size);
            dma_interrupt_action = DmaIsIdle;
        }

        // this check is if the idle runner is woken up by dma isr to create incoming task.
        // if there are other tasks to be scheduled it would make no sense to set the system
        // in sleep mode. in this case, the system would freezen for the systick interval.
        // now, os enters sleep only if the idle runner is the only one in line.
        if (running_tasks->size == 1)
            sleep();
        
        block_current_task();
    }
}

#include "runtime.h"
#include "process/scheduler.h"
#include "hw/cpu.h"
#include "memory.h"
#include "dma.h"
#include "process/task.h"
#include "uart_common.h"
#include "externals.h"

static void __attribute__((__noipa__))  __attribute__((optimize("O0"))) stat(void)
{
  while (1) {
    block_current_task();
    update_memory_statistic();
    volatile TransferInfo_t t = {.length = sizeof(MemoryStatistic_t), .start_adress = &mstat};
    uprint((unsigned int*) &t);
    SV_YIELD_TASK;
  };
}

void __attribute__((__noipa__)) __attribute__((optimize("O0"))) idle_runner(void)
{
    SV_STD;
    
    // execute non-os modules
    for (unsigned int i = 0; i < NUM_OF_EXTERNAL_FUNCTIONS; i++)
        create_task(func_ptr[i], 0);

    SV_STE;

    while (1)
    {
        if (process_stats.clean_up_requests)
            search_invalidate_tasks();

        if (dma_interrupt_action & DmaTransferedExternalTask)
        {
            create_task((void (*)()) tInfo.start_adress, (unsigned int) tInfo.start_adress);
            dma_interrupt_action = DmaIsIdle;
        }

        // this check is if the idle runner is woken up by dma isr to create incoming task.
        // if there are other tasks to be scheduled (i.e. task count > 1, because 1 is the idle_runner)
        // it would make no sense to set the system
        // in sleep mode. in this case, the system would freezen for the systick interval.
        // now, os enters sleep only if the idle runner is the only one in line.
        if (running_tasks->size == 1)
            sleep();
        
        block_current_task();
    }
}

KernelErrorCodes_t __attribute__((__noipa__))  __attribute__((optimize("O0"))) setup_kernel_runtime(void)
{
    if (init_scheduler() == -1)
        return SCHEDULER_INIT_FAILED;

    if ((kernel_pids.idle_task = create_task(&idle_runner, 0) == -1))
        return TASK_CREATION_FAILED;

    if ((kernel_pids.transfer_handler = create_task(&transfer_handler, 0)) == -1)
        return TASK_CREATION_FAILED;

    if ((kernel_pids.statistic_manager = create_task(&stat, 0)) == -1)
        return TASK_CREATION_FAILED; 

    if (run_scheduler() == -1)
        return SCHEDULER_INIT_FAILED;

    return KERNEL_INIT_SUCCEDED;
}

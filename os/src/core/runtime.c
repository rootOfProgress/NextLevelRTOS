#include "runtime.h"
#include "process/scheduler.h"
#include "hw/cpu.h"
#include "memory.h"
#include "dma.h"
#include "dma_types.h"
#include "process/task.h"
#include "uart_common.h"
#include "externals.h"
#include "uart.h"

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

void NO_OPT external_io_runner(void)
{
    while (1)
    {
        switch (state)
        {
            case PREPARE_TASK_TRANSFER:        
                tInfo.task_size = (unsigned int) *((unsigned int*) uart_rx_buffer) >> 8; 
                tInfo.start_adress = allocate(tInfo.task_size); 
                
                if (!tInfo.start_adress)
                    invoke_panic(OUT_OF_MEMORY);

                // notify host to recompile with correct offset
                print((char*) &tInfo.start_adress, 4);

                DmaTransferSpecifics_t dt;
            
                dt.chsel = 4;
                dt.minc = 1;
                dt.ndtr = tInfo.task_size;
                dt.destination_address = (unsigned int) tInfo.start_adress;
                dt.stream_number = 5;
                dt.tcie = 1;    
                dt.dma_job_type = DmaWaitsForExternalTask;
                
                dma_interrupt_action = DmaWaitsForExternalTask;
                dma_transfer(&dt, PeripherialToMemory, UART);
                break;
            case REQUEST_STATISTIC:
                wakeup_pid(kernel_pids.statistic_manager);
                break;
            case REBOOT:
                soft_reset();
                break;
            default:
                state = RX_READY;
                break;
        }
        block_current_task();
    }
}

void __attribute__((__noipa__)) __attribute__((optimize("O0"))) idle_runner(void)
{
    ST_DISABLE;
    
    // execute non-os modules
    for (unsigned int i = 0; i < NUM_OF_EXTERNAL_FUNCTIONS; i++)
        create_task(func_ptr[i], 0);

    // @todo: create subtask (function pointer) to save ram space
    kernel_pids.external_io_runner = create_task(&external_io_runner, 0);

    ST_ENABLE;

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

    if ((kernel_pids.idle_task = create_task(&idle_runner, 0)) == -1)
        return TASK_CREATION_FAILED;

    // @todo: make subtask out of it
    if ((kernel_pids.transfer_handler = create_task(&transfer_handler, 0)) == -1)
        return TASK_CREATION_FAILED;
    
    // @todo: make subtask out of it
    if ((kernel_pids.statistic_manager = create_task(&stat, 0)) == -1)
        return TASK_CREATION_FAILED; 

    if (run_scheduler() == -1)
        return SCHEDULER_INIT_FAILED;

    mstat.git_hash = GIT_HASH;
    mstat.os_version = OS_VERSION;
    mstat.magic = 0x12345678;
    mstat.debug_mode = DEBUG ? 1 : 0;
    mstat.systick_enabled = SYSTICK ? 1 : 0;

    return KERNEL_INIT_SUCCEDED;
}

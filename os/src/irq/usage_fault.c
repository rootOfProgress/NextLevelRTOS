#include "process/scheduler.h"
#include "process/task.h"
#include "hw/cpu.h"
#include "runtime.h"


void handle_kernel_error(char crashed_task)
{
    if (crashed_task == kernel_pids.idle_task)
    {
        // reboot(RebootAfterKernelError);
    }
    // switch (crashed_task)
    // {
    // case kernel_pids.idle_task: // fatal
    //     reboot();
    //     break;
    // default:
    //     // create_task()
    //     break;
    // }
}


void  __attribute__((optimize("O0"))) usage_fault(void)
{
    char* kernel_pids = (char*) &kernel_pids;
    for (unsigned int i = 0; i < sizeof(kernel_pids); i++)
    {
        if (currently_running->data->general.task_info.pid == kernel_pids[i])
            handle_kernel_error(kernel_pids[i]);
    }
    
    
    unsigned int UFSR = READ_REGISTER(0xE000ED28) >> 16;

    switch (UFSR & 0xFFFF)
    {
        case 1:// undefinestr
            break;
        case 1 << 1:// invstate
            break;
        case 1 << 2:// invpc
            break;
        case 1 << 3:// nocp
            break;
        case 1 << 8:// unaligned
            break;
        case 1 << 9:// divbyzero
            break;
        default:
            break;
    }
    while (1);
}
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
#include "health.h"
#include "irq/gpio_isr.h"

typedef enum RebootTypes RebootTypes_t;

IoChannel_t type_of_io_handler;

// @todo: Initialization does not work yet because whole structure lies in ram.
OsLifetime_t lifetime_statistic = { .version.version_number = 0, .version.size_of_struct = sizeof(OsLifetime_t) - sizeof(VersionOfStructure_t), { .0 } };

void (*io_handler) (unsigned int uart_rx_buffer);
unsigned int rx_state;

static void __attribute__((__noipa__))  __attribute__((optimize("O0"))) stat(void)
{
  while (1)
  {
    block_current_task();
    update_memory_statistic(&lifetime_statistic.memoryStat);
    update_process_statistic(&lifetime_statistic.processStat);
    volatile TransferInfo_t t = {.length = sizeof(OsLifetime_t), .start_adress = &lifetime_statistic};
    uprint((unsigned int*) &t);
    svcall(yieldTask);
  };
}

void NO_OPT external_io_runner(void)
{
  while (1)
  {
    // @todo Currently 10 IDs reserved for OS, sufficient?
    if (rx_state < 10)
    {
      switch ((UartStates_t) rx_state)
      {
      case PREPARE_TASK_TRANSFER:
        tInfo.task_size = (unsigned int) * ((unsigned int*) uart_rx_buffer) >> 8;
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
        // @todo direct call to stat currently not working, why?
        wakeup_pid(kernel_pids.statistic_manager);
        break;
      case REBOOT:
        reboot(RebootRequestedByOperator);
        break;
      default:
        rx_state = (unsigned int) RX_READY;
        break;
      }
    }
    else
    {
      if (io_handler)
      {
        io_handler((unsigned int) * ((unsigned int*) uart_rx_buffer));
      }
    }
    block_current_task();
  }
}

void __attribute__((__noipa__)) __attribute__((optimize("O0"))) idle_runner(void)
{
  init_exti_isr_handling();

  // execute non-os modules
  for (int i = 0; i < NUM_OF_EXTERNAL_FUNCTIONS; i++)
  {
    create_task(func_ptr[i], 0);
  }

  if (!(boot_flags.reboot_type == RebootRequestedByOperatorKeepStat))
  {
    memset_byte((void*) &lifetime_statistic, sizeof(OsLifetime_t), 0);
  }

  lifetime_statistic.osMeta.git_hash = GIT_HASH;
  lifetime_statistic.osMeta.os_version = OS_VERSION;
  lifetime_statistic.osMeta.magic = 0x12345678;
  lifetime_statistic.osMeta.debug_mode = DEBUG ? 1 : 0;
  lifetime_statistic.osMeta.systick_enabled = SYSTICK ? 1 : 0;

  lifetime_statistic.version.size_of_struct = 0;
  lifetime_statistic.version.size_of_struct = sizeof(OsLifetime_t) - sizeof(VersionOfStructure_t);

  update_memory_statistic(&lifetime_statistic.memoryStat);
  update_process_statistic(&lifetime_statistic.processStat);

  // @todo: create subtask (function pointer) to save ram space
  kernel_pids.external_io_runner = create_task(&external_io_runner, 0);

  type_of_io_handler = OsInternalIo;
  io_handler = NULL;

  while (1)
  {
    if (process_stats.clean_up_requests)
    {
      search_invalidate_tasks();
    }

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
    {
      sleep();
    }

    block_current_task();
  }
}

KernelErrorCodes_t __attribute__((__noipa__))  __attribute__((optimize("O0"))) setup_kernel_runtime(void)
{
  if (init_scheduler() == -1)
  {
    return SCHEDULER_INIT_FAILED;
  }

  if ((kernel_pids.idle_task = create_task(&idle_runner, 0)) == -1)
  {
    return TASK_CREATION_FAILED;
  }

  // @todo: remove this tasks
  if ((kernel_pids.transfer_handler = create_task(&transfer_handler, 0)) == -1)
    return TASK_CREATION_FAILED;

  // @todo: remove this tasks
  if ((kernel_pids.statistic_manager = create_task(&stat, 0)) == -1)
    return TASK_CREATION_FAILED;

  if (run_scheduler() == -1)
    return SCHEDULER_INIT_FAILED;

  return KERNEL_INIT_SUCCEDED;
}

// @todo: Untested
void NO_OPT hardfault_handler(void)
{
  process_stats.num_of_hardfaults++;
  invalidate_current_task();
  set_pendsv();
}

// @todo: Untested
void ISR usage_fault_handler(void)
{

  // fatal
  if (currently_running->data->general.task_info.pid == kernel_pids.idle_task)
  {
    // @todo: introduce boot flags
    reboot(RebootAfterUsageFault);
  }

  UsageFaultStatus_t* usagefault_status = (UsageFaultStatus_t*) ((unsigned int*) 0xE000ED2A);

  // activate fpu
  if (usagefault_status->nocp)
  {
    __asm(
      "LDR.W R0, =0xE000ED88\n"
      "LDR R1, [R0]\n"
      "ORR R1, R1, #(0xF << 20)\n"
      "STR R1, [R0]\n"
      "DSB\n"
      "ISB\n"
    );
    return;
  }

  // check sanity of other tasks

  invalidate_current_task();
  set_pendsv();
}
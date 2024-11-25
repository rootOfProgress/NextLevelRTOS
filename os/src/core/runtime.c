#include "runtime.h"
#include "process/scheduler.h"
#include "hw/cpu.h"
#include "memory.h"
#include "memory_shared.h"
#include "dma.h"
#include "dma_types.h"
#include "process/task.h"
#include "uart_common.h"
#include "externals.h"
#include "uart.h"
#include "shared/health.h"
#include "irq/gpio_isr.h"
#include "rtc.h"
#include "scheduler_shared.h"

typedef enum RebootTypes RebootTypes_t;

IoChannel_t type_of_io_handler;
DmaTransferSpecifics_t DmaTransferConfig;
OsErrorInformation_t osErrorLog[DEBUG ? numberOfErrorLogSlots : 0];

unsigned int osErrorCode;
static unsigned int osErrorCodeLoggingIdx = 0;

OsLifetime_t lifetime_statistic = { .version.version_number = VersionOfLifetimeInfo, 
                                    .version.size_of_struct = sizeof(OsLifetime_t) - sizeof(VersionOfStructure_t), 
                                    .osMeta.git_hash = GIT_HASH,  
                                    .osMeta.os_version = OS_VERSION,
                                    .osMeta.gcc_version = GCC_VERSION,
                                    .osMeta.magic = 0x12345678,
                                    .osMeta.debug_mode = DEBUG ? 1 : 0,
                                    .osMeta.systick_enabled = SYSTICK ? 1 : 0 };

void (*io_handler) (unsigned int uart_rx_buffer);
unsigned int rx_state;
char *generalPurposeBuffer;
void (*subtasks[maxNumOfWaitingKernelSubtasks])();

static void __attribute__((__noipa__)) __attribute__((optimize("O0"))) collectOsHealth(void)
{
  update_memory_statistic(&lifetime_statistic.memoryStat);
  update_process_statistic(&lifetime_statistic.processStat);
  print((char*) &lifetime_statistic, sizeof(OsLifetime_t));
}

void RTC_collectConvertedDateTime(void) // @todo : function needs an own space
{
  generalPurposeBuffer = (char*) allocate(sizeof(DateRepresentation_t) + sizeof(TimeRepresentation_t));

  TimeRepresentation_t currentTime;
  RTC_readCurrentTime(&currentTime);
  // print((char*) &currentTime, sizeof(TimeRepresentation_t));
  
  DateRepresentation_t currentDate;
  RTC_readCurrentDate(&currentDate);
  // print((char*) &currentDate, sizeof(DateRepresentation_t));
  
  char *helper = generalPurposeBuffer;

  memcpy_byte(helper, &currentTime, sizeof(TimeRepresentation_t));
  helper += sizeof(TimeRepresentation_t);
  memcpy_byte(helper, &currentDate, sizeof(DateRepresentation_t));
  print((char*) generalPurposeBuffer, sizeof(TimeRepresentation_t) + sizeof(DateRepresentation_t));

  deallocate((unsigned int*) &generalPurposeBuffer);
}

void writeOsError(OsErrorCodes_t error, const volatile char* functionNameSrc, unsigned int line_number)
{
  osErrorCode |= error;

  if (DEBUG)
  {
    osErrorLog[osErrorCodeLoggingIdx % numberOfErrorLogSlots].errorCode = error;

    const volatile char *helper = functionNameSrc;

    while (*helper != '\0') 
    {
      helper++;
    }

    memcpy_byte(&osErrorLog[osErrorCodeLoggingIdx % numberOfErrorLogSlots].functionName, 
                (void*) functionNameSrc, 
                helper - functionNameSrc);

    osErrorLog[osErrorCodeLoggingIdx % numberOfErrorLogSlots].lineNumber = line_number;

    osErrorCodeLoggingIdx++;
  }

}

void schedule_kernel_subtask(unsigned int task_number)
{
  unsigned int idx = 0;
  void (*subtask)() = NULL;

  while (idx++ < maxNumOfWaitingKernelSubtasks && subtasks[idx]) {}

  switch (task_number)
  {
  case statisticManager:
    subtask = collectOsHealth;
    break;
  default:
    break;
  }

  if (idx < maxNumOfWaitingKernelSubtasks)
  {
    subtasks[idx] = subtask;
  }

  if (!is_task_currently_running(kernel_pids.idle_task))
  {
    // @todo: set timer or just count down some context switches before wake up
    wakeup_pid(kernel_pids.idle_task);
  }
}

void NO_OPT external_io_runner(void)
{
  while (1)
  {
    // @todo Currently 20 IDs reserved for OS, sufficient?
    if (rx_state < 20)
    {
      switch ((UartStates_t) rx_state)
      {
      case PREPARE_TASK_TRANSFER:
        tInfo.task_size = (unsigned int) * ((unsigned int*) uart_rx_buffer) >> 8;
        tInfo.start_adress = allocate(tInfo.task_size);

        // notify host to recompile with correct offset
        print((char*) &tInfo.start_adress, 4);

        if (tInfo.start_adress)
        {
          DmaTransferConfig.ndtr = tInfo.task_size;
          DmaTransferConfig.destination_address = (unsigned int) tInfo.start_adress;
          
          dma_interrupt_action = DmaWaitsForExternalTask;

          dma_transfer(&DmaTransferConfig, PeripherialToMemory, UART);
        }
        else 
        {
          invoke_panic(OUT_OF_MEMORY);
        }
        break;
      case REQUEST_STATISTIC:
        schedule_kernel_subtask(statisticManager);
        break;
      case SET_TIME:
      case SET_DATE:
      {
        DmaTransferConfig.ndtr = sizeof(DateRepresentation_t) + sizeof(TimeRepresentation_t);
        generalPurposeBuffer = (char*) allocate(DmaTransferConfig.ndtr);
        DmaTransferConfig.destination_address = (unsigned int) generalPurposeBuffer;
        dma_interrupt_action = DmaWaitsForCurrentDateTime;
        dma_transfer(&DmaTransferConfig, PeripherialToMemory, UART);
        break;
      }
      case READ_DATETIME_RAW:
      {
        DateTimeRepresentationRaw_t dateTimeRaw;
        RTC_readCurrentDateTimeRaw(&dateTimeRaw);
        print((char*) &dateTimeRaw, sizeof(DateTimeRepresentationRaw_t));
        break;
      }
      case READ_DATETIME_CONVERTED:
      {
        RTC_collectConvertedDateTime();
        break;
      }
      case READ_ERROR_CODE:
      {
        print((char*) &osErrorCode, sizeof(unsigned int));
        break;
      }
      case READ_ERROR_LOG:
      {
        // notify host about log size
        volatile unsigned int logSize = sizeof(OsErrorInformation_t) * numberOfErrorLogSlots;
        unsigned int actualSize = sizeof(osErrorLog) / sizeof(OsErrorInformation_t);
        print((char*) &logSize, sizeof(unsigned int));
        task_sleep(500);
        if (DEBUG)
        {
          for (unsigned int logIdx = 0; logIdx < actualSize; logIdx++)
          {
            print((char*) &osErrorLog[logIdx], sizeof(OsErrorInformation_t));
          }
        }
        break;
      }
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
    if (create_task(func_ptr[i], 0) < 0)
    {
      writeOsError(OS_TaskCreationFailed, __FUNCTION__, __LINE__);
    }
  }

  update_memory_statistic(&lifetime_statistic.memoryStat);
  update_process_statistic(&lifetime_statistic.processStat);

  // @todo: create subtask (function pointer) to save ram space
  kernel_pids.external_io_runner = create_task(&external_io_runner, 0);

  if (kernel_pids.external_io_runner < 0)
  {
    writeOsError(OS_TaskCreationFailed, __FUNCTION__, __LINE__);
  }

  type_of_io_handler = OsInternalIo;
  io_handler = NULL;

  {
    unsigned int idx = 0;
    while (idx++ < maxNumOfWaitingKernelSubtasks)
    {
      subtasks[idx] = NULL;
    }
  }

  memset_byte((void*) &DmaTransferConfig, sizeof(DmaTransferSpecifics_t), 0);
  DMA_setInitialConfig(&DmaTransferConfig);

  while (1)
  {
    if (process_stats.clean_up_requests)
    {
      search_invalidate_tasks();
    }

    switch (dma_interrupt_action)
    {
      case DmaTransferedExternalTask:
      {
        if (create_task((void (*)()) tInfo.start_adress, (unsigned int) tInfo.start_adress) < 0)
        {
          writeOsError(OS_TaskCreationFailed, __FUNCTION__, __LINE__);
        }
        dma_interrupt_action = DmaIsIdle;
        break;
      }
      case DmaTransferedCurrentDateTime:
      {
        dma_interrupt_action = DmaIsIdle;
        RTC_setTimeAndDate((TimeRepresentation_t*) generalPurposeBuffer, (DateRepresentation_t*) (generalPurposeBuffer + sizeof(TimeRepresentation_t)));
        deallocate((unsigned int*) &generalPurposeBuffer);
        break;
      }
      default:
      {
        break;
      }
      // dma_interrupt_action = DmaIsIdle;
    }

    {
      unsigned int idx = 0;
      while (idx++ < maxNumOfWaitingKernelSubtasks && subtasks[idx])
      {
        subtasks[idx]();
        subtasks[idx] = NULL;
      }
    }

    // this check is if the idle runner is woken up by dma isr to create incoming task.
    // if there are other tasks to be scheduled (i.e. task count > 1, because 1 is the idle_runner)
    // it would make no sense to set the system
    // in sleep mode. in this case, the system would freezen for the systick interval.
    // now, os enters sleep only if the idle runner is the only one in line.
    if (running_tasks->size == 1)
    {
      if (!DEBUG)
      {
        sleep();
      }
    }

    block_current_task();
  }
}

KernelErrorCodes_t __attribute__((__noipa__))  __attribute__((optimize("O0"))) setup_kernel_runtime(void)
{
  if (init_scheduler() == -1)
  {
    writeOsError(OS_SchedulerInitializationFailed, __FUNCTION__, __LINE__);

    return SCHEDULER_INIT_FAILED;
  }

  if ((kernel_pids.idle_task = create_task(&idle_runner, 0)) == -1)
  {
    writeOsError(OS_TaskCreationFailed, __FUNCTION__, __LINE__);
    return TASK_CREATION_FAILED;
  }

  if (run_scheduler() == -1)
  {
    writeOsError(OS_SchedulerInitializationFailed, __FUNCTION__, __LINE__);
    return SCHEDULER_INIT_FAILED;
  }

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
void ISR_INTERNAL usage_fault_handler(void)
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
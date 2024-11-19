#include "core/exception.h"
#include "core/exception_inlines.h"
#include "core/exception_svc.h"
#include "lang.h"
#include "hw/cpu.h"
#include "process/task.h"
#include "memory.h"
#include "test.h"
#include "uart_common.h"
#include "uart.h"
#include "process/scheduler.h"
#include "runtime.h"

volatile unsigned int svc_number = 0;

void ISR_INTERNAL NO_OPT systick_isr()
{
  __asm__ volatile (
    "TST lr, #4\n"
    "ITTT NE\n"
    "MRSNE r2, PSP\n"
    "STMDBNE r2!, {r4-r11}\n"
    "MSRNE PSP, r2\n"
  );

  set_pendsv();
}

USED void uprint(UNUSED volatile unsigned int* transfer_info )
{
  svcall_param(printMsg, (unsigned int) transfer_info);
}

USED void execute_priviledged(UNUSED unsigned int function_address)
{
  svcall_param(execPriv, function_address);
}

void NO_OPT kprint(void)
{
  volatile unsigned int general_service_register;
  __asm__("mov %0, r9" : "=r"(general_service_register));

  volatile TransferInfo_t* t = (TransferInfo_t*) general_service_register;
  print((char*) t->start_adress, t->length);
}

void svcall_isr()
{  
  __asm__ volatile (
    "TST lr, #4\n"
    "ITTT NE\n"
    "MRSNE r2, PSP\n"
    "STMDBNE r2!, {r4-r11}\n"
    "MSRNE PSP, r2\n"
  );
  
  __asm__ volatile("mov %0, r6" : "=r"(svc_number));

  switch (svc_number)
  {
  case execPspTask:
    if (SYSTICK)
    {
      init_systick(1000);
     }

    Tcb_t* tcb_of_pid0 = ((Tcb_t*)currently_running->data);

    __asm__ volatile ("mov r0, %[sp]" :: [sp] "r" (tcb_of_pid0->sp));
    __asm__ volatile ("ldmia.w  r0!, {r4-r11}");
    __asm__ volatile ("msr psp, r0");
    __asm__ volatile ("mov r1, 0xfffffffd");
    __asm__ volatile ("mov lr, 0xfffffffd");
    __asm__ volatile ("bx lr");

    return;
  case printMsg:
    kprint();
  case yieldTask:
    if (DEBUG && currently_running)
    {
      Tcb_t* tcb_of_current_task = ((Tcb_t*)currently_running->data);
      tcb_of_current_task->lifetime_info[0].lifetime.voluntary_interrupts++;
    }
    set_pendsv();
    break;
  case std:
    if (SYSTICK)
    {
      ST_DISABLE;
    }
    restore_psp();
    return;
  case ste:
    if (SYSTICK)
    {
      ST_ENABLE;
    }
    restore_psp();
    return;
  case execPriv:
    unsigned int function_adress;
    __asm__ volatile ("mov %0, r9" : "=r"(function_adress));
    void (*priv_fn)() = (void (*)()) (function_adress | 1);
    priv_fn();
    restore_psp();
    return;
  case setExtIoCallback:
    unsigned int io_callback_adress;
    __asm__ volatile ("mov %0, r9" : "=r"(io_callback_adress));
    io_handler = (void (*) (unsigned int)) io_callback_adress;
    restore_psp();
    return;
  case wakeupPid:
    unsigned int pid_id;
    __asm__ volatile ("mov %0, r9" : "=r"(pid_id));
    wakeup_pid(pid_id);
    restore_psp();
    return;
  case getIoBuffer:
    unsigned int** addr;
    __asm__ volatile ("mov %0, r9" : "=r"(addr));
    *addr = (unsigned int*) &rx_state;   
    return;
  case wakeupIoHandler:
    wakeup_pid(kernel_pids.external_io_runner);
    // this sv call can be possibly invoked during an interrupt (e.g.
    // nrf2401 exti isr), so it needs to checked if there are psp registered
    // to restored or not
    restore_psp_if_threadmode();
    return;
  case disableIrReception:
    disable_irq();
    restore_psp();
    return;
  case enableIrReception:
    enable_irq();
    restore_psp();
    return;
  case getLifetime:
    // todo
    OsLifetime_t *lifetime;
    __asm__ volatile ("mov %0, r9" : "=r"(lifetime));
    
    lifetime->version.version_number = VersionOfLifetimeInfo;
    lifetime->version.size_of_struct = sizeof(OsLifetime_t) - sizeof(VersionOfStructure_t);

    asm("bkpt");
    update_memory_statistic(&lifetime->memoryStat);
    update_process_statistic(&lifetime->processStat);
    asm("bkpt");
    restore_psp();
    return;
  default:
    __builtin_unreachable();
    break;
  }
}
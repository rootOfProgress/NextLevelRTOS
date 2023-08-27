#include "process/scheduler.h"
#include "hw/cpu.h"
#include "memory.h"
#include "panic.h"

Queue_t* running_tasks = NULL;
Queue_t* waiting_tasks = NULL;
Node_t* currently_running = NULL;
Node_t* task_to_preserve = NULL;

ProcessStats_t volatile process_stats;
KernelPids_t volatile kernel_pids;
TaskSleepRequest_t volatile task_sleep_request;

void update_process_statistic(ProcessLifetime_t* process_lifetime)
{
  process_lifetime->waiting_tasks = waiting_tasks->size;

  char num_of_pending_tasks = 0;
  Node_t* q = running_tasks->head;

  for (unsigned int j = 0; j < running_tasks->size; j++)
  {
    Tcb_t* n = (Tcb_t*) currently_running->data;
    if (n->general.task_info.state == FINISHED)
    {
      num_of_pending_tasks++;
    }
  }

  process_lifetime->tasks_cleanup_pending = num_of_pending_tasks;
  process_lifetime->running_tasks = running_tasks->size - num_of_pending_tasks;
  process_lifetime->finished_tasks = process_stats.num_of_finished_tasks;
  process_lifetime->failed_tasks = process_stats.num_of_failed_tasks;
}

int init_scheduler(void)
{
  currently_running = NULL;
  running_tasks = new_queue();

  if (running_tasks == NULL)
  {
    return -1;
  }

  waiting_tasks = new_queue();

  if (waiting_tasks == NULL)
  {
    return -1;
  }

  memset_byte((void*) &process_stats, sizeof(ProcessStats_t), 0);
  memset_byte((void*) &kernel_pids, sizeof(KernelPids_t), -1);

  return 0;
}

int insert_scheduled_task(Tcb_t* tcb)
{
  switch (tcb->general.task_info.state)
  {
  case READY:
    if (enqueue_element(running_tasks, (Tcb_t*) tcb) < 0)
      return -1;
    break;
  case WAITING:
    if (enqueue_element(waiting_tasks, (Tcb_t*) tcb) < 0)
      return -1;
    break;
  default:
    return -1;
    break;
  }
  return 1;
}

void reboot(RebootTypes_t reboot_type)
{
  boot_flags.reboot_type = reboot_type;
  soft_reset();
}

void invalidate_current_task(void)
{
  ((Tcb_t*) (currently_running->data))->general.task_info.state = INVALID;
  if (DEBUG)
  {
    process_stats.num_of_failed_tasks++;
  }
  process_stats.clean_up_requests++;
}

char check_tasks_availability(unsigned int pid)
{
  Node_t *q = get_head_element(waiting_tasks);

  if (!q)
    return 0;

  for (unsigned int i = 0; i < waiting_tasks->size; i++)
  {
    if (q->data->general.task_info.pid == pid && q->data->general.task_info.state == READY)
    {
      return 1;
    }
    q = q->next;
  }

  return 0;
}

void join_task(unsigned int pid_to_wait_for)
{
  if (!check_tasks_availability(pid_to_wait_for))
  {
    return;
  }
  ((Tcb_t*) (currently_running->data))->join_pid = pid_to_wait_for;
  block_current_task();
}

void kill_all_child_tasksR(Tcb_t* parent)
{

  if (parent->child_tasks->size == 0)
  {
    return;
  }

  SingleLinkedNode_t* q = parent->child_tasks->head;
  while (q)
  {
    kill_all_child_tasksR((Tcb_t*)q->data);
    ((Tcb_t*)q->data)->general.task_info.state = INVALID;
    q = q->next;
  }
}


void kill_all_child_tasks(void)
{
  List_t* child_tasks;
  child_tasks = currently_running->data->child_tasks;

  SingleLinkedNode_t* q = child_tasks->head;
  kill_all_child_tasksR((Tcb_t*) q->data);
}


void kill_child_task(unsigned int pid_of_child, Tcb_t* parent)
{
  List_t* child_tasks;
  child_tasks = currently_running->data->child_tasks ? (List_t*) !parent : parent->child_tasks;

  if (child_tasks->size == 0)
  {
    return;
  }

  SingleLinkedNode_t* q = child_tasks->head;

  // @todo Warning, unbounded while loop!
  while (q)
  {
    if (((Tcb_t*)q->data)->general.task_info.pid == pid_of_child)
    {
      ((Tcb_t*)q->data)->general.task_info.state = INVALID;
      return;
    }
    q = q->next;
  }
  return;
}

int run_scheduler(void)
{
  if (DEBUG == 2)
  {
    timer_init(TimerForSysLogging, 0, (unsigned int[4])
    {
      0, 0, 0, 0
    }, ResolutionForSysLogging);
  }

  // set up task planer
  timer_init(TimerForTaskSleep, 0, (unsigned int[4])
  {
    0, 0, 0, 0
  }, ResolutionForSleepFunction);
  enable_ccx_ir(TimerForTaskSleep, 1);

  if (running_tasks->size == 0)
  {
    if (!(wakeup_pid(kernel_pids.idle_task)))
    {
      invoke_panic(SCHEDULER_NOT_INITIALIZED);
      return -1;
    }
  }

  currently_running = (Node_t*) get_head_element(running_tasks);

  if (!currently_running)
  {
    invoke_panic(SCHEDULER_NOT_INITIALIZED);
    return -1;
  }

  task_to_preserve = currently_running;

  return 0;
}

// workaround
void force_pid0_into_running(void)
{
  Node_t *q = get_head_element(running_tasks);

  for (unsigned int i = 0; i < running_tasks->size; i++)
  {
    if (((Tcb_t*)q->data)->general.task_info.pid == 0)
    {
      ((Tcb_t*)q->data)->general.task_info.state = READY;
      currently_running = q;
      task_to_preserve = currently_running;
      break;
    }
    q = q->next;
  }
}

void __attribute__ ((hot)) pendsv_isr(void)
{
  disable_irq();
  if (DEBUG)
  {
      process_stats.num_of_pendsv++;
      Tcb_t* tcb_of_current_task = ((Tcb_t*)currently_running->data);
  }

  __asm volatile ("mrs %0, psp" : "=r"(((Tcb_t*) task_to_preserve->data)->sp));
  switch_task();

  if (DEBUG == 2)
  {
    timer_flush_counter(TimerForSysLogging);
    timer_start(TimerForSysLogging);
  }

  __asm volatile ("mov r2, %[next_sp]":: [next_sp] "r" (((Tcb_t*) currently_running->data)->sp));
  enable_irq();
  // @todo what is that???
  __asm volatile (
    "ldmfd r2!, {r4-r11}\n"
    "mov.w r0, #3758153728\n"
    "ldr   r1, [r0, #16]\n"
    "orr.w r1, r1, #1\n"
    "str   r1, [r0, #16]\n"
    "msr psp, r2\n"
  );
}

void collect_os_statistics(char* statistic)
{
  // collect os statistics
  unsigned int num_of_all_tasks = 0;
  unsigned int i;
  num_of_all_tasks += running_tasks->size;
  num_of_all_tasks += waiting_tasks->size;

  statistic = (char*) allocate(sizeof(Tcb_t) * num_of_all_tasks + sizeof(ProcessStats_t));

  Node_t *q = running_tasks->head;

  *statistic = running_tasks->size;
  statistic += (char) 4;

  // copy into buffer
  for (i = 0; i < running_tasks->size; i++)
  {
    char *src = (char*) q->data;
    for (char j = 0; j < sizeof(Tcb_t); j++)
    {
      *(statistic) = *(src + j);
      statistic += j + i * sizeof(Tcb_t);
    }

    q = q->next;
  }

  q = waiting_tasks->head;

  *statistic = waiting_tasks->size;
  statistic += (char) 4;

  // copy into buffer
  for (i = 0; i < waiting_tasks->size; i++)
  {
    char *src = (char*) q->data;
    for (char j = 0; j < sizeof(Tcb_t); j++)
    {
      statistic += j + i * sizeof(Tcb_t);
      *(statistic) = *(src + j);
    }

    q = q->next;
  }

  char *p = (char*) &process_stats;

  for (i = 0; i < sizeof(ProcessStats_t); i++)
  {
    statistic += (char) i;
    *(statistic) = *(p + i);
  }
}


void clean_up_task(Tcb_t* t, Node_t* obsolete_node)
{
  if (t->general.task_info.is_external)
  {
    if (!deallocate((unsigned int*) t->code_section))
      invoke_panic(MEMORY_DEALLOC_FAILED);
  }

  if (!deallocate((unsigned int*) t->stacksection_lower_bound))
  {
    invoke_panic(MEMORY_DEALLOC_FAILED);
  }

  if (!deallocate((unsigned int*) t))
  {
    invoke_panic(MEMORY_DEALLOC_FAILED);
  }

  Node_t* old_element = dequeue_element(running_tasks, obsolete_node);

  if (!deallocate((unsigned int*) old_element))
    invoke_panic(MEMORY_DEALLOC_FAILED);
}

void search_invalidate_tasks(void)
{
  Node_t* q = (Node_t*) currently_running;
  for (unsigned int j = 0; j < running_tasks->size; j++)
  {
    Tcb_t* t = (Tcb_t*) q->data;
    if (t->general.task_info.state == FINISHED || t->general.task_info.state == INVALID)
    {
      clean_up_task(t, q);
      process_stats.clean_up_requests--;
    }
    q = q->next;
  }
}

void finish_task(void)
{
  currently_running->data->general.task_info.state = FINISHED;

  if (currently_running->data->parent_task->join_pid == currently_running->data->general.task_info.pid)
  {
    wakeup_pid(currently_running->data->parent_task->general.task_info.pid);
  }

  if (DEBUG)
  {
    process_stats.num_of_finished_tasks++;
  }
  process_stats.clean_up_requests++;
  set_pendsv();
}

void task_sleep(unsigned int requested_time_to_sleep)
{
  task_sleep_request.pid_of_sleeping_task = ((Tcb_t*) currently_running->data)->general.task_info.pid;
  set_ccr(TimerForTaskSleep, requested_time_to_sleep, 1);
  timer_flush_counter(TimerForTaskSleep);
  timer_start(TimerForTaskSleep);
  block_current_task();
}
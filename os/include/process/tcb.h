#ifndef TCB_H
#define TCB_H

#include "data/list.h"

typedef enum
{
  READY = 0,
  WAITING,
  FINISHED,
  DEAD,
  INVALID
} TaskState_t;

typedef union TaskInfo
{
  struct
  {
    char pid;
    char state;
    char is_external;
    unsigned short stack_size;
  } task_info;
  unsigned int raw;
} TaskInfo_t;

typedef union TaskLifetimeData
{
  struct
  {
    unsigned int voluntary_interrupts;
    unsigned int forced_interrupts;
    unsigned int cpu_time;
  } lifetime;
  unsigned int raw[3];
} TaskLifetimeData_t;

enum { IsNoExternalTask = 0, IsExternalTask = 1 };

typedef struct Tcb Tcb_t;

struct Tcb
{
  TaskInfo_t general;
  unsigned int sp;
  int codesection_lower_bound;
  int codesection_upper_bound;
  int stacksection_lower_bound;
  unsigned int code_section;
  short join_pid;
  Tcb_t* parent_task;
  List_t* child_tasks;
  TaskLifetimeData_t lifetime_info[(DEBUG == 1 || DEBUG == 2) ? 1 : 0];
};

#endif
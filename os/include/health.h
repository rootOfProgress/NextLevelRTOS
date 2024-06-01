#ifndef HEALTH_H
#define HEALTH_H

#include "common.h"

typedef struct MemoryLifetime
{
  unsigned int num_of_allocs;
  unsigned int num_of_deallocs;
  unsigned int ram_size;
  unsigned int total_byte_alloced;
  unsigned int total_byte_used;
} MemoryLifetime_t;

typedef struct OsMetaInformation
{
  unsigned int git_hash;
  unsigned int magic;
  short debug_mode;
  short systick_enabled;
  unsigned int os_version;
  unsigned int gcc_version;
} OsMetaInformation_t;

typedef struct ProcessLifetime
{
  char tasks_cleanup_pending;
  char failed_tasks;
  char finished_tasks;
  char waiting_tasks;
  char running_tasks;
} ProcessLifetime_t;

typedef struct OsLifetime
{
  VersionOfStructure_t version;
  MemoryLifetime_t memoryStat;
  OsMetaInformation_t osMeta;
  ProcessLifetime_t processStat;
} OsLifetime_t;

#endif
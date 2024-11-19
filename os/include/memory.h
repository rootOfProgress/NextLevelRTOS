#ifndef MEMORY_H
#define MEMORY_H

#define RAM_START 0x20000000

#include "runtime_types.h"
#include "shared/health.h"

typedef struct MemoryResult
{
  unsigned int* start_address;
  unsigned int* end_address;
} MemoryResult_t;

typedef struct MemoryStatisticLocal
{
  unsigned int num_of_allocs;
  unsigned int num_of_deallocs;
  unsigned int ram_size;
} MemoryStatisticLocal_t;

enum 
{
  MemoryAllocationFailed = 0
};

typedef union MemoryEntry
{
  struct
  {
    // unsigned int is_occupied : 1, is_dirty : 1, chunk_size : 15, base_offset : 16;
    unsigned int is_occupied : 1, chunk_size : 15, base_offset : 16;
    // short base_offset;
  } mementry_fields;

  unsigned int raw;
} MemoryEntry_t;

unsigned int* allocate(unsigned int);
unsigned int* allocateR(unsigned int, unsigned int, unsigned int);
unsigned int deallocate(unsigned int*);

void update_memory_statistic(MemoryLifetime_t *lifetime_info);
void init_allocator(unsigned int, unsigned int*);
void init_process_allocator(unsigned int*);
void swap(char*);
void memset_byte(void*, unsigned int, char);
void defrag(void);

int size_comparator(int, int);
int offset_comparator(int, int);

extern void lock_mutex(void * mutex);
extern void release_mutex(void * mutex);

static inline void os_memcpy(char* dst, char data)
{
  *dst = data;
}

#endif
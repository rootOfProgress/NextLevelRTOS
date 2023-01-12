#ifndef MEMORY_H
#define MEMORY_H



#define FOURBYTE 0x04

typedef struct MemoryResult {
    unsigned int* start_address;
    unsigned int* end_address;
} MemoryResult_t;

typedef struct MemoryStatistic {
    unsigned int num_of_allocs;
    unsigned int num_of_deallocs;
    unsigned int num_of_fractial_allocs;
    unsigned int total_byte_alloced;
    unsigned int total_byte_used;
    unsigned int os_data_end;
    unsigned int free_useable;
    unsigned int waiting_tasks;
    unsigned int total_scheduled_tasks;
    unsigned int cpu_load;
} MemoryStatistic_t;

unsigned int* allocate(unsigned int);
unsigned int* allocateR(unsigned int,unsigned int,unsigned int);
unsigned int deallocate(unsigned int*);
void update_memory_statistic(void);
void init_allocator(unsigned int);
void init_process_allocator(unsigned int*);
void swap(char*);
void memset_byte(void*,unsigned int, char);

extern MemoryStatistic_t mstat;

static inline void os_memcpy(char* dst, char data)
{
    *dst = data;
}

#endif
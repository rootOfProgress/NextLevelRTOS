#ifndef MEMORY_H
#define MEMORY_H

#define RAM_START 0x20000000

typedef struct MemoryResult {
    unsigned int* start_address;
    unsigned int* end_address;
} MemoryResult_t;

typedef struct MemoryStatistic {
    unsigned int num_of_allocs;
    unsigned int num_of_deallocs;
    unsigned int ram_size;
    unsigned int total_byte_alloced;
    unsigned int total_byte_used;
    unsigned int os_data_end;
    unsigned int free_useable;
    unsigned int waiting_tasks;
    unsigned int running_tasks;
    unsigned int cpu_load;
} MemoryStatistic_t;


unsigned int* allocate(unsigned int);
unsigned int* allocateR(unsigned int,unsigned int,unsigned int);
unsigned int deallocate(unsigned int*);

void update_memory_statistic(void);
void init_allocator(unsigned int,unsigned int*);
void init_process_allocator(unsigned int*);
void swap(char*);
void memset_byte(void*,unsigned int, char);
void defrag(void);

int size_comparator(int, int);
int offset_comparator(int, int);

extern MemoryStatistic_t mstat;
extern void lock_mutex(void * mutex);
extern void release_mutex(void * mutex);

static inline void os_memcpy(char* dst, char data)
{
    *dst = data;
}

#endif
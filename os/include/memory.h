#ifndef MEMORY_H
#define MEMORY_H

#define RAM_START 0x20000000

typedef struct MemoryResult {
    unsigned int* start_address;
    unsigned int* end_address;
} MemoryResult_t;

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

// 40 byte
typedef struct MemoryStatistic {
    unsigned int num_of_allocs;
    unsigned int num_of_deallocs;
    unsigned int ram_size;
    unsigned int total_byte_alloced;
    unsigned int total_byte_used;
    unsigned int git_hash;
    unsigned int magic;
    unsigned int debug_mode;
    unsigned int failed_tasks : 8, finished_tasks : 8, waiting_tasks : 8, running_tasks : 8;
    unsigned int os_version;
} MemoryStatistic_t;


unsigned int* allocate(unsigned int);
unsigned int* allocateR(unsigned int,unsigned int,unsigned int);
unsigned int deallocate(unsigned int*);

void update_memory_statistic(void);
void init_allocator(unsigned int,unsigned int*);
void init_process_allocator(unsigned int*);
void swap(char*);
void memset_byte(void*, unsigned int, char);
void defrag(void);

int size_comparator(int, int);
int offset_comparator(int, int);

extern MemoryStatistic_t volatile mstat;
extern void lock_mutex(void * mutex);
extern void release_mutex(void * mutex);

static inline void os_memcpy(char* dst, char data)
{
    *dst = data;
}

#endif
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
} MemoryStatistic_t;

unsigned int* allocate(unsigned int);
unsigned int* allocateR(unsigned int,unsigned int,unsigned int);
void deallocate(unsigned int*);
void init_allocator(unsigned int);
void swap(char*);

static inline void os_memcpy(char* dst, char data)
{
    *dst = data;
}

#endif
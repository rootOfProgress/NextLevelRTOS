#ifndef MEMORY_H
#define MEMORY_H



#define FOURBYTE 0x04

typedef struct MemoryResult {
    unsigned int* start_address;
    unsigned int* end_address;
} MemoryResult_t;

void deallocate(void*);
void init_allocator(unsigned int);
void swap(char*);

static inline void os_memcpy(char* dst, char data)
{
    *dst = data;
}

unsigned int* allocate(unsigned int);
#endif
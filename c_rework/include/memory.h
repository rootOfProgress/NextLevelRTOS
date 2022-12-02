#ifndef MEMORY_H
#define MEMORY_H



#define FOURBYTE 0x04

typedef struct MemoryResult {
    unsigned int* start_address;
    unsigned int* end_address;
} MemoryResult_t;

void deallocate(void*);
void init_allocator(unsigned int);

unsigned int* allocate(unsigned int);
#endif
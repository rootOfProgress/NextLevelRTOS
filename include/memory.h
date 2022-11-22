#ifndef MEMORY_H
#define MEMORY_H

#define FOURBYTE 0x04

typedef struct MemoryResult {
    unsigned int* start_address;
    unsigned int* end_address;
} MemoryResult_t;

void init_allocator(unsigned int);

MemoryResult_t* allocate(unsigned int);
#endif
#ifndef EXCEPTION_H
#define EXCEPTION_H

#define FOURBYTE 0x04

typedef struct MemoryResult {
    unsigned int* start_address;
    unsigned int* end_address;
} MemoryResult_t;

MemoryResult_t allocate(unsigned int);
#endif
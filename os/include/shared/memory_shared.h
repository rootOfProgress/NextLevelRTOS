#ifndef MEMORY_SHARED_H
#define MEMORY_SHARED_H

void memcpy_byte(void* dest, void* src, unsigned int number_of_bytes);
void memset_byte(void*, unsigned int, char);

unsigned int* allocate(unsigned int);
unsigned int deallocate(unsigned int*);

#endif
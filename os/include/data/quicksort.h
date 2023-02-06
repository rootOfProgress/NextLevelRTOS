#ifndef QUICKSORT_H
#define QUICKSORT_H

void quicksort(unsigned int[], int, int, int (*)(int, int));
void quicksortR(unsigned int[], int, int, int (*)(int, int));
int make_partition(unsigned int[], int, int, int (*)(int, int));
int int_comparator(int, int);
#endif
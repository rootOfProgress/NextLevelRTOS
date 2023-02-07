#ifndef QUICKSORT_H
#define QUICKSORT_H

void quicksort(int[], int, int, int (*)(int, int));
void quicksortR(int[], int, int, int (*)(int, int));
int make_partition(int[], int, int, int (*)(int, int));
int int_comparator(int, int);
#endif
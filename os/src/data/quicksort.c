#include "data/quicksort.h"
#include "memory.h"

void __attribute__((__noipa__))  __attribute__((optimize("O0"))) quicksort(int A[], int lo, int hi, int (*comparator)(int a, int b))
{
    quicksortR(A, lo, hi, comparator);
}

void swapi(int *a, int *b) {
  int t = *a;
  *a = *b;
  *b = t;
}

int __attribute__((optimize("O0"))) int_comparator(int a, int b)
{
    if (a > b)
        return 1;
    if (a < b)
        return -1;
    return 0;
}

void __attribute__((optimize("O0"))) quicksortR(int A[], int lo, int hi, int (*comparator)(int a, int b))
{
    if ((lo >= hi) || (lo < 0))
        return;
    // if (comparator(lo,hi) < 0)
    {
        int p = make_partition(A, lo, hi, comparator);
        quicksortR(A, lo, p - 1, comparator);
        quicksortR(A, p + 1, hi, comparator);
    }
}

int __attribute__((optimize("O0"))) make_partition(int A[], int lo, int hi, int (*comparator)(int a, int b))
{
    int pivot = A[hi];
    int i = lo - 1;

    for (int j = lo; j < hi ; j++)
    {
        if (comparator(A[j], pivot) == 0 || comparator(A[j], pivot) == -1)//A[j] <= pivot)
        {
            i++;
            swapi((int*) &A[i], (int*) &A[j]);
        }
    }
    swapi((int*) &A[i + 1], (int*) &A[hi]);
    return i+1;   
}
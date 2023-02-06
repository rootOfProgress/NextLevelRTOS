#include "data/quicksort.h"
#include "memory.h"

void __attribute__((__noipa__))  __attribute__((optimize("O0"))) quicksort(unsigned int A[], int lo, int hi)
{
    quicksortR(A, lo, hi);
}

void swapi(int *a, int *b) {
  int t = *a;
  *a = *b;
  *b = t;
}

void quicksortR(unsigned int A[], int lo, int hi)
{
    if ((lo >= hi) || (lo < 0))
        return;
    int p = make_partition(A, lo, hi);
    quicksortR(A, lo, p - 1);
    quicksortR(A, p + 1, hi);
}

unsigned int make_partition(unsigned int A[], int lo, int hi)
{
    int pivot = A[hi];
    int i = lo - 1;

    for (unsigned int j = lo; j < hi ; j++)
    {
        if (A[j] <= pivot)
        {
            i++;
            swapi(&A[i], &A[j]);
        }
    }
    swapi(&A[i + 1], &A[hi]);
    return i+1;   
}
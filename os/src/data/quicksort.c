#include "data/quicksort.h"
#include "memory.h"

void __attribute__((__noipa__))  __attribute__((optimize("O0"))) quicksort(unsigned int A[], int lo, int hi, int (*comparator)(int a, int b))
{
    quicksortR(A, lo, hi, comparator);
}

void swapi(int *a, int *b) {
  int t = *a;
  *a = *b;
  *b = t;
}

int int_comparator(int a, int b)
{
    if (a > b)
        return 1;
    if (a < b)
        return -1;
    return 0;
}

void quicksortR(unsigned int A[], int lo, int hi, int (*comparator)(int a, int b))
{
    if (comparator(lo, hi) > 0 || comparator(lo,hi) == 0 || comparator(lo,0) == -1)// (lo >= hi) || (lo < 0))
        return;
    int p = make_partition(A, lo, hi, comparator);
    quicksortR(A, lo, p - 1, comparator);
    quicksortR(A, p + 1, hi, comparator);
}

int make_partition(unsigned int A[], int lo, int hi, int (*comparator)(int a, int b))
{
    int pivot = A[hi];
    int i = lo - 1;

    for (unsigned int j = lo; j < hi ; j++)
    {
        if (comparator(A[j], pivot) == 0 || comparator(A[j], pivot) == -1)//A[j] <= pivot)
        {
            i++;
            swapi(&A[i], &A[j]);
        }
    }
    swapi(&A[i + 1], &A[hi]);
    return i+1;   
}
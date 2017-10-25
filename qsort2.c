#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <omp.h>

// swap A [left] and A [right]
static inline void swap
(
    double *A,
    int64_t left,
    int64_t right
)
{
    double t = A [left];
    A [left] = A [right];
    A [right] = t;
}

// in-place insertion sort on A [left:right]
static inline void isort
(
    double *A,
    int64_t left,
    int64_t right
)
{
    for (int64_t k = left; k <= right; k++)
    {
        for (int64_t j = k; j > left && A [j] < A [j-1]; j--)
        {
            // swap A [j-1] and A [j]
            swap (A, j-1, j);
        }
    }
}

// return the index of the median of 3 values: A[a],A[b],A[c]
// (returns the index a, b, or c of the median value)
static inline int64_t median
(
    double *A,
    int64_t a,
    int64_t b,
    int64_t c
)
{
    // 6 possible permutations: abc, acb, cab, bac, bca, cba
    if (A [a] < A [b])
    {
        // abc, acb, cab
        if (A [b] < A [c]) return (b);   // abc
        if (A [a] < A [c]) return (c);   // acb
        return (a);                      // cab
    }
    else
    {
        // bac, bca, cba
        if (A [a] < A [c]) return (a);   // bac
        if (A [b] < A [c]) return (c);   // bca
        return (b);                      // cba
    }
}

// C.A.R Hoare partition method, partitions an array in place via a pivot.
// k = partition (A, left, right) partitions A such that all entries
// in A [left:k] are <= all entries in A [k+1:right]
static inline int64_t partition
(
    double *A,
    int64_t left,
    int64_t right
)
{
    // find the pivot: median of 3
    double pivot = A [median (A, left, (left+right)/2, right)];

    // At the top of the while loop, A [left+1...right-1] is considered, and
    // entries outside this range are in their proper place and not touched.
    // Since the input specification of this function is to partition A
    // [left..right], left must be decremented and right incremented.
    left--;
    right++;

    // keep partitioning until the left and right sides meet
    while (true)
    {
        // loop invariant:  A [..left] < pivot and A [right..] > pivot,
        // so the region to be considered is A [left+1 ... right-1].

        // increment left until finding an entry A [left] >= pivot
        do { left++; } while (A [left] < pivot);

        // decrement right until finding an entry A [right] <= pivot
        do { right--; } while (pivot < A [right]);

        // now A [..left-1] < pivot and A [right+1..] > pivot, but
        // A [left] > pivot and A [right] < pivot, so these two entries
        // are out of place and must be swapped.

        // However, if the two sides have met, the partition is finished.
        if (left >= right)
        {
            return (right);
        }

        // since A [left] > pivot and A [right] < pivot, swap them
        swap (A, left, right);

        // after the swap this condition holds:
        // A [..left] < pivot and A [right..] > pivot
    }
}

static void quicksort_recursive
(
    double *A,
    int64_t left,
    int64_t right
)
{
    // use insertion sort on small sequences
    int64_t n = right - left + 1;
    if (n < 20)
    {
        isort (A, left, right);
        return;
    }

    // partition A [left:right] into A [left:k] and A [k+1:right]
    int64_t k = partition (A, left, right);

    if (n < 2048) 
    {
        quicksort_recursive (A, left, k);
        quicksort_recursive (A, k+1, right);
    }
    else 
    {
        // sort each partition
        #pragma omp task
        {
            quicksort_recursive (A, left, k);
        }
        #pragma omp task
        {
            quicksort_recursive (A, k+1, right);
        }
    }
}

static void quicksort
(
    double *A,
    int64_t n
)
{
    quicksort_recursive (A, 0, n-1);
}

static void quicksort_parallel
(
    double *A,
    int64_t n
)
{
    #pragma omp parallel
    #pragma omp single nowait
    {
        quicksort_recursive (A, 0, n-1);
    }
}

void dumplist
(
    double *A,
    int64_t n
)
{
    // this remains sequential:
    if (n < 8)
    {
        for (int64_t i = 0; i < n; i++)
        {
            printf ("%lld: %g\n", i, A[i]);
        }
    }
    else
    {
        for (int64_t i = 0; i < 4; i++)
        {
            printf ("%lld: %g\n", i, A[i]);
        }
        printf ("...\n");
        for (int64_t i = n-4; i < n; i++)
        {
            printf ("%lld: %g\n", i, A[i]);
        }
    }
}

int main (int argc, char **argv)
{
    printf ("argc %d\n", argc);
    // create a list to sort
    int64_t n;
    int64_t num_threads;

    if (argc <= 1) 
    {
        n = 1024;
    }
    else if (argc <= 2)
    {
        num_threads = 4;
    }
    else
    {
        sscanf (argv [1], "%lld", &n);
        sscanf (argv [2], "%lld", &num_threads);
    }
    printf ("n %lld\n", n);
    printf ("num_threads %lld\n (for parallel run)", num_threads);

    double t, t1, t2;

    // keep qsort1.c sequential; make a copy of this file,
    // call it qsort2.c, and do your modifications there.
    omp_set_num_threads (num_threads);
    t1 = omp_get_wtime ( );

    double *A = malloc (n * sizeof (double)); 
    double *B = malloc (n * sizeof (double));
    if (A == NULL || B == NULL) exit (0);

    // create the random values to sort.
    // Do this in parallel in your modified code.
    // Each iteration i uses the seed value as their thread id.
    int id = omp_get_thread_num();
    uint32_t seed = id;
    #pragma omp parallel for private(id, seed) shared(A, B)
    for (int64_t i = 0; i < n; i++)
    {
        A [i] = (double) rand_r (&seed) / ((double) RAND_MAX);
        B [i] = A[i];
    }

    t2 = omp_get_wtime ( );
    t = t2 - t1;
    printf ("time to create the lists, in seconds: %g\n", t);

    // printf ("unsorted:\n");
    // dumplist (A, n);

    // In your parallel code, make a copy of the list and
    // sort it twice: once sequentially and once in parallel.
    // Then compare the two lists to make sure they are equal.
    // Do the copy and check in parallel.

    // ---------- SEQUENTIAL

    omp_set_num_threads (1);

    t1 = omp_get_wtime ( );

    // sort the list
    // Do this in parallel in your modified code.
    quicksort (A, n);

    t2 = omp_get_wtime ( );
    t = t2-t1;
    printf ("time to sort   the list, in seconds (seq): %g\n", t);

    // ---------- PARALLEL

    omp_set_num_threads (num_threads);
    t1 = omp_get_wtime ( );

    // sort the list
    // Do this in parallel in your modified code.
    quicksort_parallel (B, n);

    t2 = omp_get_wtime ( );
    t = t2-t1;
    printf ("time to sort   the list, in seconds (par): %g\n", t);

    // printf ("sorted:\n");
    // dumplist (A, n);

    // check if the list is sorted.
    // Do this in parallel in your modified code.
    t1 = omp_get_wtime ( );
    bool ok = true;
    #pragma omp parallel for shared(ok, A, B)
    for (int64_t i = 0; i < n-1; i++)
    {
        ok = ok && (A [i] <= A [i+1]);
        ok = ok && (A[i] == B[i]);
    }
    t2 = omp_get_wtime ( );
    t = t2 - t1;
    printf ("time to check  the list, in seconds: %g\n", t);
    printf ("ok: %d\n", ok);

}

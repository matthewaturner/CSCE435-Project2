#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <omp.h>

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

static void bucketsort
(
    double *A,
    int64_t k,
    int64_t p
)
{
    // allocate Count array
    double **C;
    C = malloc(p * sizeof(double *));
    if (C == NULL) exit (0);

    #pragma omp parallel
    for (int i = 0; i < p; i++)
    {
        C[i] = malloc(k * sizeof(double));
        if (C[i] == NULL) exit(0);

        #pragma omp parallel
        for (int j = 0; j < k; j++) 
        {
            C[i][j] = 0;
        }
    }

    // count up all the values in A
    int64_t bucket_size = k/p;

    #pragma omp parallel
    for (int i=0; i<k; i+=k/p) 
    {
        int thread_num = omp_get_thread_num();
        for(int j=0; j<bucket_size && i+j<k; j++) 
        {
            C[thread_num][(int64_t)A[i+j]] += 1;
        }
    }

    double *S = malloc((k+1) * sizeof(double));

    // sum up C
    #pragma omp parallel
    for (int i=0; i<k; i++) 
    {
        S[i] = 0;
        for(int j=0; j<p; j++)
        {
            S[i] += C[j][i];
        }
    }

    // prefix sum
    int64_t sum = 0;
    for(int i=0; i<k; i++) 
    {
        S[i] += sum;
        sum = S[i];
    }

    int64_t i = 0;
    // reconstruct A
    
}


int main (int argc, char **argv)
{
    printf ("argc %d\n", argc);
    // create a list to sort
    int64_t k; // number of elements to sort
    int64_t p; // number of threads to use

    if (argc <= 1) 
    {
        k = 1024;
    }
    if (argc <= 2)
    {
        p = 4;
    }
    if (argc > 2)
    {
        sscanf (argv [1], "%lld", &k);
        sscanf (argv [2], "%lld", &p);
    }
    printf ("k %lld\n", k);
    printf ("num_threads %lld (for parallel run)\n", p);

    double t, t1, t2;

    omp_set_num_threads(p);

    // allocate A array
    double *A = malloc (k * sizeof (double)); 
    if (A == NULL) exit (0);

    int id = omp_get_thread_num();
    uint32_t seed = id;
    #pragma omp parallel for private(id, seed) shared(A)
    for (int64_t i = 0; i < k; i++)
    {
        A [i] = (double) rand_r (&seed) / ((double) RAND_MAX);
    }

    dumplist(A, k);

    t1 = omp_get_wtime ( );

    // sort the list
    // Do this in parallel in your modified code.
    bucketsort(A, k, p);

    t2 = omp_get_wtime ( );
    t = t2-t1;
    printf ("time to sort   the list, in seconds (par): %g\n", t);

    dumplist(A, k);

    t1 = omp_get_wtime ( );
    bool ok = true;
    #pragma omp parallel for shared(ok, A)
    for (int64_t i = 0; i < n-1; i++)
    {
        ok = ok && (A [i] <= A [i+1]);
    }
    t2 = omp_get_wtime ( );
    t = t2 - t1;
    printf ("time to check  the list, in seconds: %g\n", t);
    printf ("ok: %d\n", ok);
}

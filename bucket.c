#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <omp.h>

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
    for (int i=0; i<k, i+=k/p) 
    {
        int thread_num = omp_get_thread_num();
        for(int j=0; j<bucket_size && i+j<k; j++) 
        {
            C[thread_num][A[i+j]] += 1;
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
    else
    {
        sscanf (argv [1], "%lld", &k);
        sscanf (argv [2], "%lld", &p);
    }
    printf ("k %lld\n", k);
    printf ("num_threads %lld (for parallel run)\n", p);

    omp_set_num_threads(p);

    // allocate A array
    double *A = malloc (n * sizeof (double)); 
    if (A == NULL) exit (0);


}

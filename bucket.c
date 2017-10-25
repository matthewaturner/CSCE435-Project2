#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <omp.h>

static void bucketsort
(
    double *A,
    double **C,
    int64_t k
)
{
    return;
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
    else if (argc <= 2)
    {
        p = omp_get_num_threads();
    }
    else
    {
        sscanf (argv [1], "%lld", &k);
        sscanf (argv [2], "%lld", &p);
    }
    printf ("k %lld\n", p);
    printf ("num_threads %lld (for parallel run)\n", p);

    // allocate A array
    double *A = malloc (n * sizeof (double)); 
    if (A == NULL) exit (0);

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

}

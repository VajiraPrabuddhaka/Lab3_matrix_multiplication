#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <omp.h>
#include "timer.h"

int main()
{

    int n;
    double start_time, end_time;
    printf("Enter n:");
    scanf("%d", &n);

    double **matA;
    double **matB;
    double **product;

    //allocate memory to matrices
    matA = (double **)malloc(sizeof(double *) * n);
    matB = (double **)malloc(sizeof(double *) * n);
    product = (double **)malloc(sizeof(double *) * n);
    for (int i = 0; i < n; i++)
    {
        matA[i] = (double *)malloc(sizeof(double) * n);
        matB[i] = (double *)malloc(sizeof(double) * n);
        product[i] = (double *)malloc(sizeof(double) * n);
    }

    //fill with random values
    srand(time(0));
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            matA[i][j] = ((double)rand() * (200 - 0)) / (double)RAND_MAX + 0;
            matB[i][j] = ((double)rand() * (200 - 0)) / (double)RAND_MAX + 0;
            product[i][j] = 0;
        }
    }

    //perform calculation
    GET_TIME(start_time);

    #pragma omp parallel for
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            for (int k = 0; k < n; k++)
            {
                product[i][j] += matA[i][k] * matB[k][j];
            }
        }
    }
    GET_TIME(end_time);

    double total_elapsed = end_time - start_time;

    printf("\nTime elapsed: %f\n", total_elapsed);

    //free allocated memory
    for (int i = 0; i < n; i++)
    {
        double *currentIntPtr1 = matA[i];
        double *currentIntPtr2 = matB[i];
        double *currentIntPtr3 = product[i];
        free(currentIntPtr1);
        free(currentIntPtr2);
        free(currentIntPtr3);
    }
}
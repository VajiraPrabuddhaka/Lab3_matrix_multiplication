#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <omp.h>
#include "timer.h"
#include "stat.h"


int main()
{

    int n;
    double start_time, end_time;
    int m = 10;
    printf("Enter n:");
    scanf("%d", &n);

    double **matA;
    double **matB;
    double **product;
    double **transpose;

    //allocate memory to matrices
    matA = (double **)malloc(sizeof(double *) * n);
    matB = (double **)malloc(sizeof(double *) * n);
    product = (double **)malloc(sizeof(double *) * n);
    transpose = (double **)malloc(sizeof(double *) * n);
    for (int i = 0; i < n; i++)
    {
        matA[i] = (double *)malloc(sizeof(double) * n);
        matB[i] = (double *)malloc(sizeof(double) * n);
        product[i] = (double *)malloc(sizeof(double) * n);
        transpose[i] = (double *)malloc(sizeof(double) * n);
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

    //make transpose
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            transpose[i][j] = matB[j][i];
        }
    }

    //Let's find required number of samples

    double sample_mean;
    double sample_std;
    double samples[10];
    double total_elapsed;

    for (int i=0; i<10; i++){
        GET_TIME(start_time);

        #pragma omp parallel for  
        for(int p = 0; p < n; p+=m){
            for(int q = 0; q < n; q+=m){
                for(int i=0;i<n;i++){
                    for(int j = q ; j<q+m ;j++){  
                       double temp = 0; 
                       for (int k =p; k <p+m; k++) {
                         temp += matA[i][k] * transpose[j][k];;
                       }
                       product[i][j] = temp;
                    }
                }
            }
        }

        //need to have optimized algorithm here
        GET_TIME(end_time);

        total_elapsed = end_time - start_time;
        samples[i] = total_elapsed;

    }
    sample_mean = mean(samples, 10);
    sample_std = calculateSD(samples,10,sample_mean);

    int num_samples = (int) pow(( (100.0*1.960*sample_std) / (5.0*sample_mean)),2) + 1 ;

    
    //perform actual calculation of time, at +-5% accuracy and 95% confidence
    double actual_samples[num_samples];

    for (int i=0; i<num_samples; i++){
        GET_TIME(start_time);

        #pragma omp parallel for  
        for(int p = 0; p < n; p+=m){
            for(int q = 0; q < n; q+=m){
                for(int i=0;i<n;i++){
                    for(int j = q ; j<q+m ;j++){  
                       double temp = 0; 
                       for (int k =p; k <p+m; k++) {
                         temp += matA[i][k] * transpose[j][k];;
                       }
                       product[i][j] = temp;
                    }
                }
            }
        }
        
        GET_TIME(end_time);

        total_elapsed = end_time - start_time;
        actual_samples[i] = total_elapsed;

    }

    
    printf("\nTotal samples required : %i\n", num_samples);
    printf("Time elapsed average: %f\n", mean(actual_samples,num_samples));
    

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


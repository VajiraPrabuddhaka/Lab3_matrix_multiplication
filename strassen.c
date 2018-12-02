#include<stdio.h>
#include<stdlib.h>
#include <time.h>
#include <math.h>
#include <omp.h>
#include "timer.h"
#include "stat.h"

double** create(int,int);
double** createZeroMatrix(int);
double** strassensMultiplication(double **, double **, int);

double** strassensMultRec(double **, double**, int n);
double** divide(double ** ,int n, int row,int col);
void printMatrix(double **, int);
double ** addMatrix(double**,double**,int);
double** subMatrix(double**,double**,int);
void compose(double**,double**,int,int,int);

/*
* Main function where the execution starts.
*/
int main(int argc, char ** argv) {
	
	int n;
    double start_time, end_time;
    printf("Enter n:");
    scanf("%d", &n);

    //double **matA;
    //double **matB;
    double **product;

   
    printf("done");
	//To handle when n is not power of k we do the padding with zero
	int power = 1;
	while(power<n){
		power=power*2;
	}

	printf("done");

	double** matA = create(n, power);
	double** matB = create(n, power);
	//result = create(n, power);

	printf("done");
	

	n = power;
	
	//Let's find required number of samples

    double sample_mean;
    double sample_std;
    double samples[10];
    double total_elapsed;

    for (int i=0; i<10; i++){
        GET_TIME(start_time);

        product = strassensMultiplication(matA, matB, n);
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

        product = strassensMultiplication(matA, matB,n);
        //optimized one
        
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

	return 0;
}

/*
* Create the matrix with random values
*/
double ** create(int n,int power){
	double ** array = createZeroMatrix(power);	
	srand(time(0));
	int i,j;
	for(i = 0;i < n; i++) {
   	 	for(j = 0; j < n; j++) {
	    		array[i][j] = ((double)rand() * (200 - 0)) / (double)RAND_MAX + 0;
			}
	}
	return array;
} 

/*
* This function creates the matrix and initialize all elements to zero
*/
double ** createZeroMatrix(int n){
	double ** array = (double**)malloc(n*sizeof(double *));	
	int i,j;
	for(i = 0;i < n; i++) {
	    	array[i] = (double*)malloc(n*sizeof(double *));
   	 		for(j = 0; j < n; j++) {
	        	array[i][j] = 0.0;
	    	}
	}
	return array;
} 
/*
* Function to Print Matrix 
*/
void printMatrix(double ** matrix,int n) {
	int i,j;
	for(i=0;i<n;i++){
		for(j=0;j<n;j++){
			printf("   %.2f   ",matrix[i][j]);
		}
		printf("\n");
	}
}


/*
* Wrapper function over strassensMultRec.
*/
double ** strassensMultiplication(double ** matrixA, double** matrixB, int n){
	double ** result = strassensMultRec(matrixA,matrixB,n);
	return result;
}

/*
* Strassen's Multiplication algorithm using Divide and Conquer technique.
*/
double** strassensMultRec(double ** matrixA, double** matrixB, int n){
	double ** result = createZeroMatrix(n);
	
	if(n>1) {
		//Divide the matrix
		double ** a11 = divide(matrixA, n, 0, 0);
		double ** a12 = divide(matrixA, n, 0, (n/2));
		double ** a21 = divide(matrixA, n, (n/2), 0);
		double ** a22 = divide(matrixA, n, (n/2), (n/2));	
		double ** b11 = divide(matrixB, n, 0, 0);
		double ** b12 = divide(matrixB, n, 0, n/2);
		double ** b21 = divide(matrixB, n, n/2, 0);
		double ** b22 = divide(matrixB, n, n/2, n/2);
		
		//Recursive call for Divide and Conquer
		double** m1= strassensMultRec(addMatrix(a11,a22,n/2),addMatrix(b11,b22,n/2),n/2);
		double** m2= strassensMultRec(addMatrix(a21,a22,n/2),b11,n/2);
		double** m3= strassensMultRec(a11,subMatrix(b12,b22,n/2),n/2);
		double** m4= strassensMultRec(a22,subMatrix(b21,b11,n/2),n/2);
		double** m5= strassensMultRec(addMatrix(a11,a12,n/2),b22,n/2);
		double** m6= strassensMultRec(subMatrix(a21,a11,n/2),addMatrix(b11,b12,n/2),n/2);
		double** m7= strassensMultRec(subMatrix(a12,a22,n/2),addMatrix(b21,b22,n/2),n/2);

		double** c11 = addMatrix(subMatrix(addMatrix(m1,m4,n/2),m5,n/2),m7,n/2);
		double** c12 = addMatrix(m3,m5,n/2);
		double** c21 = addMatrix(m2,m4,n/2);
		double** c22 = addMatrix(subMatrix(addMatrix(m1,m3,n/2),m2,n/2),m6,n/2);
		//Compose the matrix
		compose(c11,result,0,0,n/2);
		compose(c12,result,0,n/2,n/2);
		compose(c21,result,n/2,0,n/2);
		compose(c22,result,n/2,n/2,n/2);
	} 
	else {
		//This is the terminating condition for recurssion.
		result[0][0]=matrixA[0][0]*matrixB[0][0];
	}
	return result;
}

/*
* This method combines the matrix in the result matrix
*/
void compose(double** matrix,double** result,int row,int col,int n){
	int i,j,r=row,c=col;
	for(i=0;i<n;i++){
		c=col;
		for(j=0;j<n;j++){
			result[r][c]=matrix[i][j];	
			c++;	
		}
		r++;
	}
}

/*
* Sub-divide the matrix according to row and col specified 
*/
double** divide(double ** matrix,int n, int row,int col) {
	int n_new=n/2;	
	
	double ** array = createZeroMatrix(n_new);	
	int i,j,r=row,c=col;
	for(i = 0;i < n_new; i++) {
		c=col;
   	 	for(j = 0; j < n_new; j++) {
        		array[i][j] = matrix[r][c];
			c++;
    		}
		r++;
	}
	return array;
}

/*
* Add the two input matrix
*/
double** addMatrix(double** matrixA,double** matrixB,int n){
	double ** res = createZeroMatrix(n);
	int i,j;
	#pragma omp parallel for	
	for(i=0;i<n;i++)
		for(j=0;j<n;j++)
			res[i][j]=matrixA[i][j]+matrixB[i][j];
	
	return res;
}
/*
* Substract the two matrix
*/
double** subMatrix(double** matrixA,double** matrixB,int n){
	double ** res = createZeroMatrix(n);
	int i,j;
	#pragma omp parallel for	
	for(i=0;i<n;i++)
		for(j=0;j<n;j++)
			res[i][j]=matrixA[i][j]-matrixB[i][j];
	
	return res;
}
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

double mean(double values[], int size){
    double sum = 0.0;
    for (int i=0; i<size; i++){
        sum = sum + values[i];
    }
    return sum/(double) size;
}

double calculateSD(double data[],int size, double mean)
{
    double standardDeviation = 0.0;

    for(int i=0; i<size; i++){
        standardDeviation += pow(data[i] - mean, 2);
    }

    return sqrt(standardDeviation/(double) size);
}
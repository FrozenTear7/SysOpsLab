#include <stdio.h>
#include <stdlib.h>

void printAllocs(int n) {
    float *arr1, *arr2;

    arr1 = (float *) malloc(n * sizeof(float));
    arr2 = (float *) calloc(n, sizeof(float));

    for (int i = 0; i < n; i++) {
        printf("%f", arr1[i]);
        puts("\n");
    }

    puts("\n");

    for (int i = 0; i < n; i++) {
        printf("%f", arr2[i]);
        puts("\n");
    }

    free(arr1);
    free(arr2);
}
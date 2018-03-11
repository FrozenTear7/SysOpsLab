#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define GLOBALARR 1000

char globalArr[GLOBALARR][GLOBALARR];

// n jest wielkoscia tablicy
char **createArray(unsigned int n) {
    char **arr = (char **) calloc(n, sizeof(char *));

    return arr;
}

void deleteArray(char **arr, int n) {
    int i;
    for (i = 0; i < n; i++) {
        free(arr[i]);
    }

    free(arr);
}

// n to rozmiar bloku, i to indeks w podanej tablicy
void createBlock(char **arr, char *data, int i, unsigned int blockSize) {
    int j;

    arr[i] = calloc(blockSize, sizeof(char));
    for (j = 0; j < blockSize; j++) {
        arr[i][j] = data[j];
    }
}

void createGlobalBlock(char *data, int i, unsigned int blockSize) {
    int j;
    for (j = 0; j < blockSize; j++) {
        globalArr[i][j] = data[j];
    }
}

void deleteBlock(char **arr, int i) {
    if (arr[i])
        free(arr);
}

int getArrayCharSum(char *arr, int n) {
    int sum = 0, i;

    for (i = 0; i < n; i++) {
        sum += arr[i];
    }

    return sum;
}

char *blockSumClosestToNumber(char **arr, int number, int n) {
    int i, sumDiff = INT_MAX;
    char *closestArr = arr[0];

    for (i = 1; i < n; i++) {
        int tmpSum = getArrayCharSum(arr[i], n);

        if (abs(tmpSum - number) < sumDiff) {
            closestArr = arr[i];
            sumDiff = abs(tmpSum - number);
        }
    }

    return closestArr;
}

int getGlobalArrayCharSum(int i, int n) {
    int sum = 0, j;

    for (j = 0; j < n; j++) {
        sum += globalArr[i][j];
    }

    return sum;
}

char *getGlobalBlock(int i, int n) {
    int j;
    char * arr;
    arr = calloc(n, sizeof(char));

    for(j = 0; j < n; j++) {
        arr[j] = globalArr[i][j];
    }

    return arr;
}

char *globalBlockSumClosestToNumber(int number, int n) {
    int i, sumDiff = INT_MAX;
    char *closestArr = getGlobalBlock(0, n);

    for (i = 1; i < n; i++) {
        int tmpSum = getGlobalArrayCharSum(i, n);

        if (abs(tmpSum - number) < sumDiff) {
            closestArr = getGlobalBlock(i, n);
            sumDiff = abs(tmpSum - number);
        }
    }

    return closestArr;
}
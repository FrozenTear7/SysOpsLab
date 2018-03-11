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

void deleteArray(char **arr) {
    int i, n = sizeof(arr) / sizeof(arr[0]);

    for (i = 0; i < n; i++) {
        free(arr[i]);
    }

    free(arr);
}

// n to rozmiar bloku, i to indeks w podanej tablicy
void createBlock(char **arr, char *data, int i) {
    int j;
    unsigned int n = sizeof(data) / sizeof(data[0]);

    arr[i] = (char *) calloc(n, sizeof(char));
    for (j = 0; j < n; j++) {
        arr[i][j] = data[j];
    }
}

// n to rozmiar bloku, i to indeks w globalnej tablicy
void *createGlobalBlock(char *data, int i) {
    int j, n = sizeof(data) / sizeof(data[0]);

    if (i < GLOBALARR) {
        for (j = 0; j < n; j++) {
            globalArr[i][j] = rand() % 100;
        }
    }
}

void deleteBlock(char **arr, int i) {
    if (arr[i])
        free(arr);
}

int getArrayCharSum(char *arr) {
    int sum = 0, i, n = sizeof(arr) / sizeof(char);

    for (i = 0; i < n; i++) {
        sum += arr[i];
    }

    return sum;
}

char *blockSumClosestToNumber(char **arr, int number) {
    int i, sumDiff = INT_MAX, n = sizeof(arr) / sizeof(arr[0]);
    char *closestArr = arr[0];

    for (i = 1; i < n; i++) {
        int tmpSum = getArrayCharSum(arr[i]);

        if (tmpSum - number < sumDiff && tmpSum - number >= 0) {
            closestArr = arr[i];
            sumDiff = tmpSum;
        }
    }

    return closestArr;
}
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define GLOBALARR 1000

char globalArr[GLOBALARR][GLOBALARR];

// n jest wielkoscia tablicy
char **createArray(unsigned int n) {
    char ** arr;
    arr = calloc(n, sizeof(char *));

    return arr;
}

void deleteArray(char **arr) {
    int i, n = sizeof(arr) + 1;

    for (i = 0; i < n; i++) {
        free(arr[i]);
    }

    free(arr);
}

// n to rozmiar bloku, i to indeks w podanej tablicy
void createBlock(char **arr, char *data, int i) {
    int j;
    unsigned int n = sizeof(data) + 1;

    arr[i] = calloc(n, sizeof(char));
    for (j = 0; j < n; j++) {
        arr[i][j] = data[j];
    }
}

void deleteBlock(char **arr, int i) {
    if (arr[i])
        free(arr);
}

int getArrayCharSum(char *arr) {
    int sum = 0, i, n = sizeof(arr) + 1;

    for (i = 0; i < n; i++) {
        sum += arr[i];
    }

    return sum;
}

char *blockSumClosestToNumber(char **arr, int number) {
    int i, sumDiff = INT_MAX, n = sizeof(arr) + 1;
    char *closestArr = arr[0];

    for (i = 1; i < n; i++) {
        int tmpSum = getArrayCharSum(arr[i]);

        if (abs(tmpSum - number) < sumDiff) {
            closestArr = arr[i];
            sumDiff = abs(tmpSum - number);
        }
    }

    return closestArr;
}
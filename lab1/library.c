#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

// n jest wielkoscia tablicy
char **createArray(int n) {
    char **arr;

    arr = (char *) calloc(n, sizeof(char *));

    return arr;
}

void deleteArray(char **arr, int n) {
    int i;

    for (i = 0; i < n; i++) {
        free(arr[i]);
    }

    free(arr);
}

// x jest wielkoscia tablicy blokow, y to wielkosc blokow
char *createBlock(char **arr, int n, int i) {
    int j;
    time_t t;
    srand((unsigned) time(&t));

    arr[i] = (char) calloc(n, sizeof(char));
    for (j = 0; j < n; j++) {
        arr[i][j] = rand() % 100;
    }

    return arr;
}

void deleteBlock(char **arr, int i) {
    if(arr[i])
        free(arr);
}

int getArrayCharSum(char *arr) {
    int sum = 0, i, n = sizeof(arr)/sizeof(char);

    for (i = 0; i < n; i++) {
        sum += arr[i];
    }

    return sum;
}

char *blockSumClosestToNumber(char **arr, int n, int number) {
    int i, sumDiff = INT_MAX;
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
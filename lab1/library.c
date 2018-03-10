#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

// n jest wielkoscia tablicy
char *createArray(int n) {
    char *arr;

    arr = (char *) calloc(n, sizeof(char));

    return arr;
}

void deleteArray(char *arr) {
    free(arr);
}

// x jest wielkoscia tablicy blokow, y to wielkosc blokow
char **createBlockArray(int x, int y) {
    char **arr;
    int i;

    arr = calloc(x, sizeof(char *));
    for (i = 0; i < x; i++) {
        arr[i] = calloc(y, sizeof(char));
    }

    return arr;
}

void deleteBlockArray(char **arr, int n) {
    int i;

    for (i = 0; i < n; i++) {
        free(arr[i]);
    }

    free(arr);
}

int getArrayCharSum(char *arr, int n) {
    int sum = 0, i;

    for (i = 0; i < n; i++) {
        sum += arr[i];
    }

    return sum;
}

char *blockSumClosestToNumber(char **arr, int n, int number) {
    int i, sumDiff = INT_MAX;
    char *closestArr = arr[0];

    for (i = 1; i < n; i++) {
        int tmpSum = getArrayCharSum(arr[i], n);

        if (tmpSum - number < sumDiff && tmpSum - number >= 0) {
            closestArr = arr[i];
            sumDiff = tmpSum;
        }
    }

    return closestArr;
}
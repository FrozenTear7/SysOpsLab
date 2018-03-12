#include <stdio.h>
#include <time.h>
#include "libblocks.h"

int main(int argc, char **argv) {
    printf("%d", argc);
    int k;

    // parse arguments

    printf("\nStworzenie tablicy o wielkosci 100 blokow, kazdy blok rozmiaru 100 znakow.\n");

    int i, j, closestNumber = 500, startDelete = 10, endDelete = 15;
    unsigned int arrSize = 100, blockSize = 100;
    char **testArr = createArray(arrSize);
    time_t t;
    srand((unsigned) time(&t));

    for (i = 0; i < arrSize; i++) {
        char *tmp;
        tmp = calloc(blockSize, sizeof(char));
        for (j = 0; j < blockSize; j++) {
            tmp[j] = (char) ((rand() % 26) + 65);
        }

        createBlock(testArr, tmp, i, blockSize);
    }

    printf("\nOdszukanie bloku o sumie elementow najblizszej danej liczbie.\n");

    char *closestArr = blockSumClosestToNumber(testArr, closestNumber, blockSize);

    printf("\nUsuniecie a nastepnie ponowne dodanie blokow.\n");

    for (i = startDelete; i < endDelete; i++) {
        deleteBlock(testArr, i);
    }

    for (i = startDelete; i < endDelete; i++) {
        char *tmp;
        tmp = calloc(blockSize, sizeof(char));
        for (j = 0; j < blockSize; j++) {
            tmp[j] = (char) ((rand() % 26) + 65);
        }

        createBlock(testArr, tmp, i, blockSize);
    }

    printf("\nNaprzemienne usuwanie i dodawanie blokow.\n");

    for (i = startDelete; i < endDelete; i++) {
        deleteBlock(testArr, i);
        char *tmp;
        tmp = calloc(blockSize, sizeof(char));
        for (j = 0; j < blockSize; j++) {
            tmp[j] = (char) ((rand() % 26) + 65);
        }

        createBlock(testArr, tmp, i, blockSize);
    }

    return 0;
}
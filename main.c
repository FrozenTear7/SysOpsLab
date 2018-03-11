#include <stdio.h>
#include <time.h>
#include "libblocks.h"

void printTime() {

}

int main() {
    FILE *raport = fopen("raport2.txt", "w");
    if (raport == NULL) {
        printf("Error opening file!\n");
        return 1;
    }

    printf("\nStworzenie tablicy o wielkosci 100 blokow, kazdy blok rozmiaru 100 znakow.\n");

    int i, j, closestNumber = 500, startDelete = 10, endDelete = 50;
    unsigned int arrSize = 100, blockSize = 100;
    char **testArr = createArray(arrSize);
    time_t t;
    srand((unsigned) time(&t));

    for (i = 0; i < arrSize; i++) {
        char *tmp = (char *) calloc(blockSize, sizeof(char));
        for (j = 0; j < blockSize; j++) {
            tmp[j] = (char) (rand() % 128);
        }

        createBlock(testArr, tmp, i);
    }

    printf("\nOdszukanie bloku o sumie elementow najblizszej danej liczbie.\n");

    char *closestArr = blockSumClosestToNumber(testArr, closestNumber);

    for (i = 0; i < sizeof(closestArr) / sizeof(closestArr[0]); i++) {
        printf("%c", closestArr[i]);
    }

    printf("\nUsuniecie a nastepnie ponowne dodanie blokow.\n");

    

    for(i = startDelete; i < endDelete; i++) {
        char *tmp = (char *) calloc(blockSize, sizeof(char));
        for (j = 0; j < blockSize; j++) {
            tmp[j] = (char) (rand() % 128);
        }

        createBlock(testArr, tmp, i);
    }

    printf("Naprzemienne usuwanie i dodawanie blokow.\n");

    for(i = startDelete; i < endDelete; i++) {
        deleteBlock(testArr, i);
        char *tmp = (char *) calloc(blockSize, sizeof(char));
        for (j = 0; j < blockSize; j++) {
            tmp[j] = (char) (rand() % 128);
        }

        createBlock(testArr, tmp, i);
    }

    return 0;
}
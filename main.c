#include <stdio.h>
#include <time.h>
#include "libblocks.h"
#include <unistd.h>
#include <sys/times.h>
#include <sys/resource.h>

void showTime(struct timeval userTime,	struct timeval systemTime, struct timespec *realTime, struct rusage *usage, FILE *f){
	clock_gettime(CLOCK_REALTIME, realTime);
	getrusage(RUSAGE_SELF, usage);
	userTime = usage->ru_utime;
	systemTime = usage->ru_stime;
	printf("\nCurrent time:\nReal time:%lld\nUser time:%ld\nSystem time:%ld\n",
		(long long) realTime->tv_sec * 1000000 + realTime->tv_nsec/1000,
		userTime.tv_sec * 1000000 + userTime.tv_usec,
		systemTime.tv_sec * 1000000 + systemTime.tv_usec
	);
	fprintf(f, "\nCurrent time:\nReal time:%lld\nUser time:%ld\nSystem time:%ld\n",
		(long long) realTime->tv_sec * 1000000 + realTime->tv_nsec/1000,
		userTime.tv_sec * 1000000 + userTime.tv_usec,
		systemTime.tv_sec * 1000000 + systemTime.tv_usec
	);
}

int main() { 
	struct timespec * realTime = malloc(sizeof(struct timespec));
	struct timeval userTime;
	struct timeval systemTime;
	struct rusage * usage = malloc(sizeof(struct timespec));

    FILE *f = fopen("raport2.txt", "w");
    if (f == NULL) {
        printf("Error opening file!\n");
        return 1;
    }

    printf("\nStworzenie tablicy o wielkosci 100 blokow, kazdy blok rozmiaru 100 znakow.\n");
    fprintf(f, "\nStworzenie tablicy o wielkosci 100 blokow, kazdy blok rozmiaru 100 znakow.\n");
	showTime(userTime, systemTime, realTime, usage, f);

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

        createBlock(testArr, tmp, i);
    }

    printf("\nOdszukanie bloku o sumie elementow najblizszej danej liczbie.\n");
    fprintf(f, "\nOdszukanie bloku o sumie elementow najblizszej danej liczbie.\n");
	showTime(userTime, systemTime, realTime, usage, f);

    char *closestArr = blockSumClosestToNumber(testArr, closestNumber);
/*
    printf("\nUsuniecie a nastepnie ponowne dodanie blokow.\n");
    fprintf(f, "\nUsuniecie a nastepnie ponowne dodanie blokow.\n");
	showTime(userTime, systemTime, realTime, usage, f);

    for (i = startDelete; i < endDelete; i++) {
        deleteBlock(testArr, i);
    }

    for (i = startDelete; i < endDelete; i++) {
        char *tmp;
        tmp = calloc(blockSize, sizeof(char));
        for (j = 0; j < blockSize; j++) {
            tmp[j] = (char) ((rand() % 26) + 65);
        }

        createBlock(testArr, tmp, i);
    }

    printf("\nNaprzemienne usuwanie i dodawanie blokow.\n");
    fprintf(f, "\nNaprzemienne usuwanie i dodawanie blokow.\n");
	showTime(userTime, systemTime, realTime, usage, f);

    for (i = startDelete; i < endDelete; i++) {
        deleteBlock(testArr, i);
        char *tmp;
        tmp = calloc(blockSize, sizeof(char));
        for (j = 0; j < blockSize; j++) {
            tmp[j] = (char) ((rand() % 26) + 65);
        }

        createBlock(testArr, tmp, i);
    }
*/
    fclose(f);

    return 0;
}
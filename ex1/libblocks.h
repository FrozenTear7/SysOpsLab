#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

char **createArray(unsigned int n);

void deleteArray(char **arr, int n);

void createBlock(char **arr, char *data, int i, unsigned int blockSize);

void createGlobalBlock(char *data, int i, unsigned int blockSize);

void deleteBlock(char **arr, int i);

int getArrayCharSum(char *arr, int n);

char *blockSumClosestToNumber(char **arr, int number, int n);

int getGlobalArrayCharSum(int i, int n);

char *getGlobalBlock(int i, int n);

char *globalBlockSumClosestToNumber(int number, int n);
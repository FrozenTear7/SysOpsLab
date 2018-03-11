#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

char **createArray(unsigned int n);

void deleteArray(char **arr);

void createBlock(char **arr, unsigned int n, int i);

void deleteBlock(char **arr, int i);

int getArrayCharSum(char *arr);

char *blockSumClosestToNumber(char **arr, int number);
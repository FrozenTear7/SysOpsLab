#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

char *createArray(int n);

void deleteArray(char *arr);

char **createBlockArray(int x, int y);

void deleteBlockArray(char **arr, int n);

int getArrayCharSum(char *arr, int n);

char *blockSumClosestToNumber(char **arr, int n, int number);
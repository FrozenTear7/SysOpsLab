#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

char **createArray(unsigned int n);

void deleteArray(char **arr, int n);

void createBlock(char **arr, char *data, int i);

void createGlobalBlock(char *data, int i);

void deleteBlock(char **arr, int i);

void deleteGlobalBlock(char **arr, int i);

int getArrayCharSum(char *arr);

int getGlobalArrayCharSum(char *arr);

char *blockSumClosestToNumber(char **arr, int number);
#include <stdio.h>
#include <stdlib.h>

int main()
{
    char **testArr = malloc(100 * sizeof(char*));
    for(int i = 0; i < 100; i++) {
        testArr[i] = malloc(100 * sizeof(char));
    }

    for(int i = 0; i < 100; i++) {
        free(testArr[i]);
    }

    free(testArr);
    return 0;
}
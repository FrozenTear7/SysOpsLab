#include <stdio.h>
#include <stdlib.h>

int main()
{
    char **testArr = malloc(10000 * sizeof(char*));
    for(int i = 0; i < 10000; i++) {
        testArr[i] = malloc(10000 * sizeof(char));
        testArr[i][i] = 'a';
        printf("Mem alloc loop iteration %d\n", i);
    }

    free(testArr);
    return 0;
}
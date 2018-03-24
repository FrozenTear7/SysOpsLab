#include <stdio.h>
#include <stdlib.h>

int main()
{
    char **testArr = malloc(1000 * sizeof(char*));
    printf("xd");
    for(int i = 0; i < 1000; i++) {
        testArr[i] = malloc(1000 * sizeof(char));
        printf("Mem alloc loop iteration %d\n", i);
    }

    free(testArr);
    return 0;
}
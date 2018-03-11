#include <stdio.h>
#include "libblocks.h"

int main() {
    char **arr1 = createArray(4);
    int i, j;
    for (i = 0; i < 4; i++) {
        createBlock(arr1, 4, i);
        for (j = 0; j < 4; j++) {
            printf("%c\n", arr1[0][j]);
        }

        printf("\n%c", blockSumClosestToNumber(arr1, 50)[0]);
    }

    deleteArray(arr1);

    return 0;
}
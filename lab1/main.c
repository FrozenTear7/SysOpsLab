#include <stdio.h>
#include "library.h"

int main() {
    char **arr1 = createArray(4);
    createBlock(arr1, 0, 4);
    int i;
    for(i = 0; i < 4; i++) {
        printf("%d", arr1[0][i]);
    }
    deleteArray(*arr1);

    return 0;
}
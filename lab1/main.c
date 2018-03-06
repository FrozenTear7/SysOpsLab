#include <stdio.h>
#include "ex1.h"
#include "ex1shared.h"

int main() {
    int n;

    puts("Podaj ilosc elementow n: ");
    scanf("%d", &n);

    printAllocs(n);

    char *arr1 = createArray(4);
    deleteArray(arr1);

    return 0;
}
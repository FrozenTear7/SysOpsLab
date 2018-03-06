#include <stdio.h>
#include "library1.c"

int main() {
    int n;

    puts("Podaj ilosc elementow n: ");
    scanf("%d", &n);

    printAllocs(n);

    return 0;
}
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    printf("Number of args: %d\n", argc - 1);

    for (int i = 1; i < argc; i++)
    {
        printf("Arg nr %d: %d\n", i, atoi(argv[i]));
    }

    return 0;
}
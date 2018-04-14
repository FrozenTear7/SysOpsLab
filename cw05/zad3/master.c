#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
    mkfifo(argv[1], 0666);

    FILE *fp = fopen(argv[1], "r");

    char buf[100];

    while (fgets(buf, sizeof(buf), fp) != NULL)
    {
        printf("%s\n", buf);
    }

    remove(argv[1]);

    return 0;
}
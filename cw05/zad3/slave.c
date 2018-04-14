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
    FILE *fp = fopen(argv[1], "w");
    fwrite("hello world2", 1, sizeof("hello world2"), fp);
    fwrite("hello world", 1, sizeof("hello world"), fp);

    return 0;
}
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
    FILE *fp = open(argv[1], "r");

    sleep(5);

    char buffer[100];
    while (fgets(buffer, 100, fp) != NULL)
    {
        printf("%s", buffer);
    }

    return 0;
}
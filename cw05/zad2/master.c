#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int slavesReady = 0;

void slavesReadyHandler(int signo)
{
    if (signo == SIGRTMIN)
        slavesReady = 1;
}

int main(int argc, char **argv)
{
    printf("xd");
    signal(SIGRTMIN, slavesReadyHandler);

    char *myfifo = "fifo2";

    if (mkfifo(myfifo, 0666) == -1)
    {
        printf("Error creating fifo.txt\n");
        remove(myfifo);
        return 1;
    }

    printf("xd");
    FILE *fp = open(argv[1], "r");
    if (fp == NULL)
    {
        perror("Master - Error opening fifo.txt");
        remove(fp);
        exit(1);
    }
    printf("xd");
/*
    while (slavesReady == 0)
    {
    }
*/
    char buffer[100];
    printf("xd");
    while (fgets(buffer, 100, fp) != NULL)
    {
        printf("%s", buffer);
    }

    close(fp);

    return 0;
}
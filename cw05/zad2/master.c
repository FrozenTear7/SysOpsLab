#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int slavesReady = 0;

void slavesReadyHandler(int signo) {
    if (signo == SIGRTMIN)
        slavesReady = 1;
}

int main(int argc, char **argv) {
    signal(SIGRTMIN, slavesReadyHandler);
    mkfifo(argv[1], 0666);

    FILE *fp = fopen(argv[1], "r");

    while (slavesReady == 0) {
    }

    char mystring[100];
    if (fp == NULL) {
        printf("Error opening fifo.txt");
        exit(1);
    }

    while (fgets(mystring, 100, fp) != NULL)
        puts(mystring);

    remove(argv[1]);

    fclose(fp);

    return 0;
}
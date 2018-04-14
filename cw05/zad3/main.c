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

int slavesReady = 0;

void slavesHandler(int signo)
{
    if (signo == SIGRTMIN + 1)
        slavesReady++;
}

int main(int argc, char **argv)
{
    if (argc < 2)
        return 1;

    signal(SIGRTMIN + 1, slavesHandler);

    char *fifo = "fifo";

    char *masterArgv[3];
    masterArgv[0] = "./master";
    masterArgv[1] = fifo;
    masterArgv[2] = NULL;

    char *slaveArgv[3];
    slaveArgv[0] = "./slave";
    slaveArgv[1] = fifo;
    slaveArgv[2] = argv[1];
    slaveArgv[3] = NULL;

    pid_t master = fork();
    if (master == 0)
    {
        execvp(masterArgv[0], masterArgv);
    }

    for (int i = 0; i < atoi(argv[1]); i++)
    {
        pid_t slave = fork();
        if (slave == 0)
        {
            execvp(slaveArgv[0], slaveArgv);
        }
    }

    while (slavesReady < atoi(argv[1]))
    {
    }

    kill(master, SIGRTMIN);

    waitpid(master, NULL, 0);

    return 0;
}
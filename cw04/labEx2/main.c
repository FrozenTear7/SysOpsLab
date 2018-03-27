#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

int counter = 1;

void handle_sigusr1(int signum)
{
    printf("Otrzymano sygnal nr: %d\n", counter++);
}

int main()
{
    signal(SIGUSR1, handle_sigusr1);
    int pid = fork();

    if (pid > 0)
    {
        while (1)
            kill(pid, SIGUSR1);
    }
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>

int doneProcesses = 0;
int n, k;
int *pidArr;

void exit_program()
{
    for(int j = 0; j < n; j++)
        kill(pidArr[doneProcesses-1], SIGINT);

    exit(0);
}

void allow()
{
    if (running)
    {
        kill(pid, SIGINT);
        running = 0;
        printf("\nOczekuję na CTRL+Z - kontynuacja albo CTRL+C - zakonczenie programu\n");
    }
    else
    {
        started = 1;
    }
}

void send()
{
    kill(getppid(), SIGRTMIN + (rand() % (SIGRTMAX - SIGRTMIN)));
}

int main(int argc, char **argv)
{
    if (!argv[1] || !argv[2])
        return 1;
    
    n = atoi(argv[1]);
    k = atoi(argv[2]);
    
    pidArr = malloc(atoi(argv[1]) * sizeof(int));
    int i = 0;

    while (doneProcesses < n)
    {
        int pid = fork();

        if (pid > 0)
        {
            pidArr[i] = pid;
            i++;
           
            signal(SIGINT, exit_program);
            signal(SIGUSR1, allow);
        }
        else if (pid == 0)
        {
            int sleepTime = (rand%10)+1;
            sleep(sleepTime);
            
            kill(getppid(), SIGUSR1);
            
            signal(SIGUSR1, send);
            
            pause();
            
            exit(1);
        }
    }

    return 0;
}

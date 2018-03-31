#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

int running = 1;
int pid;
int childPid;

void exit_program()
{
    printf("\nOdebrano sygnał SIGINT\n");

    if (childPid)
        kill(childPid, SIGINT);

    exit(0);
}

void handle_sig()
{
    kill(childPid, SIGINT);

    printf("\nOczekuję na CTRL+Z - kontynuacja albo CTRL+C - zakonczenie programu\n");
}

int main()
{
    struct sigaction act;
    act.sa_handler = handle_sig;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGTSTP, &act, NULL);
    signal(SIGINT, exit_program);

    int pid = fork();

    if (pid == 0)
    {
        childPid = getpid();

        while (1)
        {
            //printf("Childpid: %d\n", childPid);
            execlp("date", "date", NULL);

            //sleep(1);
        }
    }

    return 0;
}
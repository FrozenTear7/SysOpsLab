#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>

int running = 0;
int started = 1;
int pid;

void exit_program()
{
    printf("\nOdebrano sygnał SIGINT\n");

    if (running)
        kill(pid, SIGINT);

    exit(0);
}

void handle_sig()
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

int main(int argc, char **argv)
{
    if (!argv[1] || !argv[2])
        return 1;

    while (1)
    {
        if (!running && started)
        {
            pid = fork();
            running = 1;
            started = 0;
        }

        if (pid > 0)
        {
            struct sigaction act;
            act.sa_handler = handle_sig;
            sigemptyset(&act.sa_mask);
            act.sa_flags = 0;
            sigaction(SIGTSTP, &act, NULL);
            signal(SIGINT, exit_program);

            wait(NULL);
        }
        else if (pid == 0)
        {
            execlp("./infDate.sh", "./infDate.sh", NULL);
        }
    }

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

int running = 1;

void exit_program()
{
    printf("\nOdebrano sygnał SIGINT\n");
    exit(0);
}

void handle_sig()
{
    running = running ? 0 : 1;

    printf("\nOczekuję na CTRL+Z - kontynuacja albo CTRL+C - zakonczenie programu\n");
}

int main()
{
    int pid = fork();

    if (pid == 0)
    {
        struct sigaction act;
        act.sa_handler = handle_sig;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;
        sigaction(SIGTSTP, &act, NULL);
        signal(SIGINT, exit_program);

        while (1)
        {
            if (running)
            {
                execlp("date", "date", NULL);
            }

            sleep(1);
        }
    }
    else
    {
        //pause();
    }

    return 0;
}
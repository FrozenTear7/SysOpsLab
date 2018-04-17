#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

int running = 1;

void exit_program()
{
    printf("\nOdebrano sygnał SIGINT");
    exit(0);
}

void handle_sig()
{
    //running = running ? 0 : 1;

    if (running)
    {
        running = 0;
        printf("\nOczekuję na CTRL+Z - kontynuacja albo CTRL+C - zakonczenie programu\n");
    }
    else
    {
        running = 1;
    }
}

int main()
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
            time_t now;
            struct tm *now_tm;
            now = time(NULL);
            now_tm = localtime(&now);

            printf("Current hour: %d:%d:%d\n", now_tm->tm_hour, now_tm->tm_min, now_tm->tm_sec);
        }

        sleep(1);
    }

    return 0;
}
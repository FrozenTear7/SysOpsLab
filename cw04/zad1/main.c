#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

bool running = true;

void exit_program()
{
    printf("Odebrano sygnał SIGINT");
    exit(0);
}

void handle_sig()
{
    if (!running)
        running = true;
    else
        running = false;

    printf("Oczekuję na CTRL+Z - kontynuacja albo CTRL+C - zakonczenie programu\n");

    struct sigaction act;
    act.sa_handler = handle_sig;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGTSTP, &act, NULL);
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
        if (running)
        {
            time_t now;
            struct tm *now_tm;
            now = time(NULL);
            now_tm = localtime(&now);

            printf("Current hour: %d:%d:%d\n", now_tm->tm_hour, now_tm->tm_min, now_tm->tm_sec);
            sleep(1);
        }
}
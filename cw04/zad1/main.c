#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

void handle_sig(int signum)
{
    if (signum == SIGINT)
        exit(1);
    else if (signum == SIGTSTP)
        printf("Oczekuję na CTRL+Z - kontynuacja albo CTRL+C - zakonczenie programu\n");
}

int main()
{
    signal(SIGINT, handle_sig);
    struct sigaction act;
    act.sa_handler = handle_sig;
    sigaction(SIGTSTP, &act, NULL);

    while (1)
    {
        time_t now;
        struct tm *now_tm;
        now = time(NULL);
        now_tm = localtime(&now);

        printf("Current hour: %d:%d:%d\n", now_tm->tm_hour, now_tm->tm_min, now_tm->tm_sec);
        sleep(1);
    }
}
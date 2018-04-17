#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>

int n, type;
int childReceived = 0;
int parentReceived = 0;

void childSend(int signum)
{
    childReceived++;
    kill(getppid(), SIGUSR1);
}

void childSendRealTime(int signum)
{
    childReceived++;
    kill(getppid(), SIGRTMIN);
}

void childEnd(int signum)
{
    printf("\n>Child< Received %d signals\n", childReceived);
    exit(0);
}

void parentReceive(int signum)
{
    parentReceived++;
}

int main(int argc, char **argv)
{
    if (!argv[1] || !argv[2])
        return 1;

    n = atoi(argv[1]);
    type = atoi(argv[2]);

    if (type < 1 || type > 3)
        return 1;

    int pid = fork();

    if (pid != 0)
    {
        if (type == 1)
        {
            signal(SIGUSR1, parentReceive);

            for (int i = 0; i < n; i++)
            {
                sleep(1);
                kill(pid, SIGUSR1);
            }

            printf("\n>Parent< Sent %d signals\n", n);
            printf("\n>Parent< Received %d signals\n", parentReceived);
            kill(pid, SIGUSR2);
            waitpid(pid, NULL, 0);
        }
        else if (type == 2)
        {

            signal(SIGUSR1, parentReceive);

            for (int i = 0; i < n; i++)
            {
                sleep(1);
                kill(pid, SIGUSR1);
                if (i != n - 1)
                    pause();
            }

            printf("\n>Parent< Sent %d signals\n", n);
            printf("\n>Parent< Received %d signals\n", parentReceived);
            kill(pid, SIGUSR2);
            waitpid(pid, NULL, 0);
        }
        else
        {
            signal(SIGRTMIN, parentReceive);

            for (int i = 0; i < n; i++)
            {
                sleep(1);
                kill(pid, SIGRTMIN);
            }

            printf("\n>Parent< Sent %d signals\n", n);
            printf("\n>Parent< Received %d signals\n", parentReceived);
            kill(pid, SIGRTMIN + 1);
            waitpid(pid, NULL, 0);
        }
    }
    else if (pid == 0)
    {
        if (type == 1 || type == 2)
        {
            signal(SIGUSR1, childSend);
            signal(SIGUSR2, childEnd);
        }
        else if (type == 3)
        {
            signal(SIGRTMIN, childSendRealTime);
            signal(SIGRTMIN + 1, childEnd);
        }

        while (1)
        {
        }
    }

    return 0;
}

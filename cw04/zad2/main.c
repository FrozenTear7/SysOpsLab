#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>

int doneProcesses = 0;
int waitingProcesses = 0;
int n, k;
int *pidArr;
int *waitArr;

void exit_program(int signum, siginfo_t *siginfo, void *context)
{
    printf("MOTHERSHIP: Received SIGINT from PID %d!\n", siginfo->si_pid);

    for (int i = 0; i < n; i++)
    {
        if (pidArr[i] != -1)
        {
            kill(pidArr[i], SIGINT);
            waitpid(pidArr[i], NULL, 0);
        }
    }

    exit(0);
}

void allow(int signum, siginfo_t *siginfo, void *context)
{
    printf("MOTHERSHIP: Received SIGUSR1 from PID %d!\n", siginfo->si_pid);

    if (waitingProcesses == k)
    {
        kill(siginfo->si_pid, SIGUSR1);
        //waitpid(siginfo->si_pid, NULL, 0);
    }
    else
    {
        waitArr[waitingProcesses++] = siginfo->si_pid;
        if (waitingProcesses == k)
        {
            for (int i = 0; i < k; i++)
            {
                kill(waitArr[i], SIGUSR1);
                //waitpid(waitArr[i], NULL, 0);
            }
        }
    }
}

void send(int signum)
{
    kill(getppid(), SIGRTMIN + (rand() % (SIGRTMAX - SIGRTMIN)));
    exit(0);
}

void realTime(int signum, siginfo_t *siginfo, void *context)
{
    printf("MOTHERSHIP: Received real time signal: SIGMIN+%i from pid: %d\n", signum - SIGRTMIN, siginfo->si_pid);
    doneProcesses++;
}

int main(int argc, char **argv)
{
    if (!argv[1] || !argv[2])
        return 1;

    n = atoi(argv[1]);
    k = atoi(argv[2]);

    pidArr = malloc(atoi(argv[1]) * sizeof(int));
    waitArr = malloc(atoi(argv[1]) * sizeof(int));
    int i = 0;
    time_t t;
    srand((unsigned)time(&t));

    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;

    act.sa_sigaction = allow;
    sigaction(SIGUSR1, &act, NULL);

    act.sa_sigaction = exit_program;
    sigaction(SIGINT, &act, NULL);

    for (int i = SIGRTMIN; i <= SIGRTMAX; i++)
    {
        act.sa_sigaction = realTime;
        sigaction(i, &act, NULL);
    }

    while (i < n)
    {
        int pid = fork();

        if (pid != 0)
        {
            pidArr[i] = pid;
            i++;

            wait(NULL);
        }
        else if (pid == 0)
        {
            int sleepTime = (rand() % 10) + 1;
            sleep(sleepTime);

            printf("%d slept for: %d\n", getpid(), sleepTime);

            kill(getppid(), SIGUSR1);

            signal(SIGUSR1, send);

            pause();

            exit(1);
        }
    }

    while (doneProcesses < n)
    {
        sleep(3);
    };

    return 0;
}

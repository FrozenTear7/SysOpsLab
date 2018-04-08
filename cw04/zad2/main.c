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
int lastPid = -1;

void exit_program(int signum, siginfo_t *siginfo, void *context)
{
    printf(">Parent process< Received SIGINT from PID %d!\n", siginfo->si_pid);

    for (int i = 0; i < n; i++)
    {
        if (pidArr[i] != -1)
        {
            kill(pidArr[i], SIGINT);
        }
    }

    exit(0);
}

void allow(int signum, siginfo_t *siginfo, void *context)
{
    printf(">Parent process< Received SIGUSR1 from PID %d!\n", siginfo->si_pid);

    if (waitingProcesses == k)
    {
        lastPid = siginfo->si_pid;
        kill(siginfo->si_pid, SIGUSR1);
        printf(">Parent process< Sent SIGUSR1 to PID %d!\n", siginfo->si_pid);
    }
    else
    {
        waitArr[waitingProcesses++] = siginfo->si_pid;
        if (waitingProcesses == k)
        {
            lastPid = siginfo->si_pid;

            for (int i = 0; i < k; i++)
            {
                kill(waitArr[i], SIGUSR1);
                printf(">Parent process< Sent SIGUSR1 to PID %d!\n", siginfo->si_pid);
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
    printf(">Parent process< Received real time signal: SIGMIN+%i from pid: %d\n", signum - SIGRTMIN, siginfo->si_pid);
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
        if (lastPid != -1)
            waitpid(lastPid, NULL, NULL);

        int pid = fork();

        if (pid == 0)
        {
            time_t t;
            srand((unsigned)time(&t));

            signal(SIGUSR1, send);

            printf(">%d< created\n", getpid());

            int sleepTime = (rand() % 10) + 1;
            sleep(sleepTime);

            printf(">%d< slept for: %d\n", getpid(), sleepTime);

            kill(getppid(), SIGUSR1);

            pause();

            exit(0);
        }
        else if (pid != 0)
        {
            pidArr[i] = pid;
            i++;

            pause();
        }
    }

    while (doneProcesses < n)
    {
        sleep(1);
    };

    free(pidArr);
    free(waitArr);

    return 0;
}

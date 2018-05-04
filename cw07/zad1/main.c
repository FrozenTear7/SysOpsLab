#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>

#include "info.h"

key_t fifoKey;
Fifo *fifo = NULL;
int semId = -1;
int shmID = -1;

int counter = 0;
sigset_t fullMask;

void sigintHandler(int signum) {
    exit(1);
}

void sigrtminHandler(int signum) {
    counter++;
}

int takePlace() {
    int barberStat = semctl(semId, 0, GETVAL);

    pid_t myPID = getpid();

    if (barberStat == 0) {
        struct sembuf sops;

        sops.sem_num = BARBER;
        sops.sem_op = 1;
        sops.sem_flg = 0;

        semop(semId, &sops, 1);
        printf("%d wakes barber up, Time: %ld\n", myPID, timeMs());
        semop(semId, &sops, 1);

        fifo->chair = myPID;

        return 1;
    } else {
        int res = fifoPush(fifo, myPID);

        if (res == -1) {
            printf("%d No free place, Time: %ld\n", myPID, timeMs());
            return -1;
        } else {
            printf("%d in queue, Time: %ld\n", myPID, timeMs());
            return 0;
        }
    }
}

void atexitHandler() {
    shmdt(fifo);
}

int main(int argc, char **argv) {
    if (argc < 3)
        return 0;

    atexit(atexitHandler);
    signal(SIGINT, sigintHandler);
    signal(SIGRTMIN, sigrtminHandler);

    fifoKey = ftok(getenv("HOME"), keyId);
    shmID = shmget(fifoKey, 0, 0);
    fifo = (Fifo *) shmat(shmID, NULL, 0);

    semId = semget(fifoKey, 0, 0);

    sigfillset(&fullMask);
    sigdelset(&fullMask, SIGRTMIN);
    sigdelset(&fullMask, SIGINT);

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGRTMIN);
    sigprocmask(SIG_BLOCK, &mask, NULL);

    for (int i = 0; i < atoi(argv[1]); i++) {
        pid_t id = fork();

        if (id == 0) {
            while (counter < atoi(argv[2])) {
                struct sembuf sops;

                sops.sem_num = CHECKER;
                sops.sem_op = -1;
                sops.sem_flg = 0;
                semop(semId, &sops, 1);

                sops.sem_num = FIFO;
                semop(semId, &sops, 1);

                int res = takePlace();

                sops.sem_op = 1;
                semop(semId, &sops, 1);

                sops.sem_num = CHECKER;
                semop(semId, &sops, 1);

                if (res != -1) {
                    sigsuspend(&fullMask);
                    printf("%d cut, Time: %ld\n", getpid(), timeMs());
                }
            }

            return 0;
        }
    }

    while (1) {
        wait(NULL);

        if (errno == ECHILD)
            break;
    }

    return 0;
}

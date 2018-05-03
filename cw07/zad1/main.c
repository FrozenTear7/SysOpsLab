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

#include "fifo.h"

key_t fifoKey;
Fifo *fifo = NULL;
int shmID = -1;
int semId = -1;
int counter = 0;
sigset_t fullMask;

long getMicroTime() {
    struct timespec marker;
    clock_gettime(CLOCK_MONOTONIC, &marker);

    return marker.tv_nsec / 1000;
}

void rtminHandler(int signum) {
    counter++;
}

void fifoSet() {
    fifoKey = ftok(getenv("HOME"), 5);

    shmID = shmget(fifoKey, 0, 0);

    void *tmp = shmat(shmID, NULL, 0);
    fifo = (Fifo *) tmp;
}

void semaphoreInit() {
    semId = semget(fifoKey, 0, 0);
}

void atexitClear() {
    shmdt(fifo);
}

void intHandler(int signum) {
    exit(1);
}

int takePlace() {
    int barberStat = semctl(semId, 0, GETVAL);

    pid_t myPID = getpid();

    if (barberStat == 0) {
        struct sembuf sops;
        sops.sem_num = 0;
        sops.sem_op = 1;
        sops.sem_flg = 0;

        semop(semId, &sops, 1);
        long timeMarker = getMicroTime();
        printf("Time: %ld, Client %d has awakened barber!\n", timeMarker, myPID);
        semop(semId, &sops, 1);

        fifo->chair = myPID;

        return 1;
    } else {
        int res = fifoPush(fifo, myPID);
        if (res == -1) {
            long timeMarker = getMicroTime();
            printf("Time: %ld, Client %d couldnt find free place!\n", timeMarker, myPID);
            return -1;
        } else {
            long timeMarker = getMicroTime();
            printf("Time: %ld, Client %d took place in the queue!\n", timeMarker, myPID);
            return 0;
        }
    }
}

int main(int argc, char **argv) {
    if (argc < 3)
        return 0;

    signal(SIGINT, intHandler);
    signal(SIGRTMIN, rtminHandler);
    atexit(atexitClear);
    fifoSet();
    semaphoreInit();

    sigfillset(&fullMask);
    sigdelset(&fullMask, SIGINT);
    sigdelset(&fullMask, SIGRTMIN);

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGRTMIN);
    sigprocmask(SIG_BLOCK, &mask, NULL);

    for (int i = 0; i < atoi(argv[1]); i++) {
        pid_t child = fork();

        if (child == 0) {
            while (counter < atoi(argv[2])) {
                struct sembuf sops;

                sops.sem_num = 2;
                sops.sem_op = -1;
                sops.sem_flg = 0;
                semop(semId, &sops, 1);

                sops.sem_num = 1;
                semop(semId, &sops, 1);

                int res = takePlace();

                sops.sem_op = 1;
                semop(semId, &sops, 1);

                sops.sem_num = 2;
                semop(semId, &sops, 1);

                if (res != -1) {
                    sigsuspend(&fullMask);
                    long timeMarker = getMicroTime();
                    printf("Time: %ld, Client %d just got cut!\n", timeMarker, getpid());
                    fflush(stdout);
                }
            }

            exit(0);
        }
    }

    while(1){
        wait(NULL);
    }

    return 0;
}
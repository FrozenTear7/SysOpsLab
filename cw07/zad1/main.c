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

void intHandler(int signo) {
    exit(2);
}

void prepareFifo();

void prepareSemafors();

void prepareFullMask();

void freeResources(void);

int takePlace();

void getCut(int ctsNum);

key_t fifoKey;
Fifo *fifo = NULL;
int semId = -1;
int shmID = -1;

volatile int ctsCounter = 0;
sigset_t fullMask;

void rtminHandler(int signo) {
    ctsCounter++;
}

int main(int argc, char **argv) {
    if (argc < 3)
        return 0;

    atexit(freeResources);
    signal(SIGINT, intHandler);
    signal(SIGRTMIN, rtminHandler);

    prepareFifo();
    prepareSemafors();
    prepareFullMask();

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGRTMIN);
    sigprocmask(SIG_BLOCK, &mask, NULL);

    for (int i = 0; i < atoi(argv[1]); i++) {
        pid_t id = fork();

        if (id == 0) {
            getCut(atoi(argv[2]));
            return 0;
        }
    }

    printf("All clients has been bred!\n");
    while (1) {
        wait(NULL); // czekaj na dzieci
        if (errno == ECHILD) break;
    }

    return 0;
}

void getCut(int ctsNum) {
    while (ctsCounter < ctsNum) {
        struct sembuf sops;
        sops.sem_num = CHECKER;
        sops.sem_op = -1;
        sops.sem_flg = 0;
        if (semop(semId, &sops, 1) == -1) throww("Client: taking checker failed!");

        sops.sem_num = FIFO;
        if (semop(semId, &sops, 1) == -1) throww("Client: taking FIFO failed!");

        int res = takePlace();

        sops.sem_op = 1;
        if (semop(semId, &sops, 1) == -1) throww("Client: releasing FIFO failed!");

        sops.sem_num = CHECKER;
        if (semop(semId, &sops, 1) == -1) throww("Client: releasing checker failed!");

        if (res != -1) {
            sigsuspend(&fullMask);
            long timeMarker = timeMs();
            printf("Time: %ld, Client %d just got cut!\n", timeMarker, getpid());
            fflush(stdout);
        }
    }
}

int takePlace() {
    int barberStat = semctl(semId, 0, GETVAL);
    if (barberStat == -1) throww("Client: getting value of BARBER sem failed!");

    pid_t myPID = getpid();

    if (barberStat == 0) {
        struct sembuf sops;
        sops.sem_num = BARBER;
        sops.sem_op = 1;
        sops.sem_flg = 0;

        if (semop(semId, &sops, 1) == -1) throww("Client: awakening barber failed!");
        long timeMarker = timeMs();
        printf("Time: %ld, Client %d has awakened barber!\n", timeMarker, myPID);
        fflush(stdout);
        if (semop(semId, &sops, 1) == -1) throww("Client: awakening barber failed!");

        fifo->chair = myPID;

        return 1;
    } else {
        int res = fifoPush(fifo, myPID);
        if (res == -1) {
            long timeMarker = timeMs();
            printf("Time: %ld, Client %d couldnt find free place!\n", timeMarker, myPID);
            fflush(stdout);
            return -1;
        } else {
            long timeMarker = timeMs();
            printf("Time: %ld, Client %d took place in the queue!\n", timeMarker, myPID);
            fflush(stdout);
            return 0;
        }
    }
}

void prepareFifo() {
    char *path = getenv(env);
    if (path == NULL) throww("Breeder: Getting enviromental variable failed!");

    fifoKey = ftok(path, keyId);
    if (fifoKey == -1) throww("Breeder: getting key of shm failed!");

    shmID = shmget(fifoKey, 0, 0);
    if (shmID == -1) throww("Breeder: opening shm failed!");

    void *tmp = shmat(shmID, NULL, 0);
    if (tmp == (void *) (-1)) throww("Breeder: attaching shm failed!");
    fifo = (Fifo *) tmp;
}

void prepareSemafors() {
    semId = semget(fifoKey, 0, 0);
    if (semId == -1) throww("Breeder: opening semafors failed!");
}

void prepareFullMask() {
    sigfillset(&fullMask);
    sigdelset(&fullMask, SIGRTMIN);
    sigdelset(&fullMask, SIGINT);
}

void freeResources() {
    shmdt(fifo);
}

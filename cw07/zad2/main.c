#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/msg.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "info.h"

void prepareFifo();

void prepareSemafors();

int takePlace();

void getCut(int ctsNum);

Fifo *fifo = NULL;
sem_t *BARBER;
sem_t *FIFO;
sem_t *CHECKER;
sem_t *SLOWER;
int counter = 0;
sigset_t sigMask;

void sigintHandler(int signum) {
    exit(1);
}

void sigrtminHandler(int signum) {
    counter++;
}

void atexitHandler() {
    munmap(fifo, sizeof(fifo));
    sem_close(BARBER);
    sem_close(FIFO);
    sem_close(CHECKER);
    sem_close(SLOWER);
}

int main(int argc, char **argv) {
    if (argc < 3)
        return 0;

    atexit(atexitHandler);
    signal(SIGINT, sigintHandler);
    signal(SIGRTMIN, sigrtminHandler);

    int shmID = shm_open(shmPath, O_RDWR, 0666);
    fifo = (Fifo *) mmap(NULL, sizeof(Fifo), PROT_READ | PROT_WRITE, MAP_SHARED, shmID, 0);

    BARBER = sem_open(barberPath, O_RDWR);
    FIFO = sem_open(fifoPath, O_RDWR);
    CHECKER = sem_open(checkerPath, O_RDWR);
    SLOWER = sem_open(slowerPath, O_RDWR);

    for (int i = 0; i < atoi(argv[1]); i++) {
        pid_t id = fork();

        if (id == 0) {
            while (counter < atoi(argv[2])) {
                sem_wait(CHECKER);

                sem_wait(FIFO);

                int res = takePlace();

                printf("%d sit, Time: %ld\n", getpid(), timeMs());

                sem_post(FIFO);

                sem_post(CHECKER);

                if (res != -1) {
                    sigsuspend(&sigMask);
                    printf("%d cut, Time: %ld\n", getpid(), timeMs());
                }
            }

            printf("%d leave, Time: %ld\n", getpid(), timeMs());

            return 0;
        }
    }

    while (1) {
        wait(NULL);

        if (errno == ECHILD)
            break;
    }

    exit(0);
}

void getCut(int ctsNum) {
    while (counter < ctsNum) {
        if (sem_wait(CHECKER) == -1) throww("Client: taking checker failed!");

        if (sem_wait(FIFO) == -1) throww("Client: taking FIFO failed!");

        int res = takePlace();

        if (sem_post(FIFO) == -1) throww("Client: releasing FIFO failed!");

        if (sem_post(CHECKER) == -1) throww("Client: releasing checker failed!");

        if (res != -1) {
            sigsuspend(&sigMask);
            long timeMarker = timeMs();
            printf("Time: %ld, Client %d just got cut!\n", timeMarker, getpid());
            fflush(stdout);
        }
    }
}

int takePlace() {
    int barberStat;
    sem_getvalue(BARBER, &barberStat);

    pid_t myPID = getpid();

    if (barberStat == 0) {
        if (sem_post(BARBER) == -1) throww("Client: awakening barber failed!");
        long timeMarker = timeMs();
        printf("Time: %ld, Client %d has awakened barber!\n", timeMarker, myPID);
        fflush(stdout);
        if (sem_wait(SLOWER) == -1)
            throww("Client: waiting for barber failed!"); // waiting for barber to set his value to 1

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
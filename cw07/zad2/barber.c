#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <semaphore.h>

#include "info.h"

Fifo *fifo = NULL;

sem_t *BARBER;
sem_t *CLIENTS;
sem_t *BLOCK;

void cut(pid_t pid) {
    printf("Start cutting %d, Time: %ld\n", pid, timeMs());

    kill(pid, SIGRTMIN);

    printf("End cutting %d, Time: %ld\n", pid, timeMs());
}

pid_t takeChair() {
    sem_wait(CLIENTS);

    pid_t nextClient = fifo->chair;

    printf("Sit %d, Time: %ld\n", nextClient, timeMs());

    sem_post(CLIENTS);

    return nextClient;
}

void work() {
    while (1) {
        sem_wait(BARBER);
        sem_post(BARBER);

        printf("AWAKENING, Time: %ld\n", timeMs());

        pid_t nextClient = takeChair();

        cut(nextClient);

        while (1) {
            sem_wait(CLIENTS);

            nextClient = fifoPop(fifo);

            if (nextClient != -1) {
                printf("Sit %d, Time: %ld\n", nextClient, timeMs());

                sem_post(CLIENTS);

                cut(nextClient);
            } else {
                printf("SLEEP, Time: %ld\n", timeMs());

                sem_wait(BARBER);

                sem_post(CLIENTS);

                break;
            }
        }
    }
}

void atexitHandler() {
    munmap(fifo, sizeof(fifo));
    shm_unlink(pathShm);
    sem_close(BARBER);
    sem_unlink(pathBarber);
    sem_close(CLIENTS);
    sem_unlink(pathFifo);
    sem_close(BLOCK);
    sem_unlink(pathBlock);
}

void sigintHandler(int signum) {
    exit(1);
}

int main(int argc, char **argv) {
    if (argc < 2)
        return 0;

    atexit(atexitHandler);
    signal(SIGINT, sigintHandler);

    int shmID = shm_open(pathShm, O_CREAT | O_EXCL | O_RDWR, 0666);
    ftruncate(shmID, sizeof(Fifo));

    fifo = (Fifo *) mmap(NULL, sizeof(Fifo), PROT_READ | PROT_WRITE, MAP_SHARED, shmID, 0);
    fifoInit(fifo, atoi(argv[1]));

    BARBER = sem_open(pathBarber, O_CREAT | O_EXCL | O_RDWR, 0666, 0);
    CLIENTS = sem_open(pathFifo, O_CREAT | O_EXCL | O_RDWR, 0666, 1);
    BLOCK = sem_open(pathBlock, O_CREAT | O_EXCL | O_RDWR, 0666, 1);

    work();

    exit(0);
}

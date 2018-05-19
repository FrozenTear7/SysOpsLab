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

void work() {
    fifo->asleep = 0;

    while (1) {
        if (fifoEmpty(fifo)) {
            printf("SLEEP, Time: %ld\n", timeMs());
            fifo->asleep = 1;
        }

        sem_wait(CLIENTS);
        sem_wait(BLOCK);

        if(fifo->asleep == 1) {
            printf("AWAKENING, Time: %ld\n", timeMs());
            fifo->asleep = 0;
        }

        pid_t tmp = fifoPop(fifo);

        if (tmp != -1) {
            printf("Sit %d, Time: %ld\n", tmp, timeMs());

            fifo->chair = tmp;
            pid_t nextClient = fifo->chair;

            printf("Start cutting %d, Time: %ld\n", nextClient, timeMs());

            sem_post(BARBER);
            sem_post(BLOCK);

            printf("End cutting %d, Time: %ld\n", nextClient, timeMs());
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
    CLIENTS = sem_open(pathFifo, O_CREAT | O_EXCL | O_RDWR, 0666, 0);
    BLOCK = sem_open(pathBlock, O_CREAT | O_EXCL | O_RDWR, 0666, 1);

    work();

    exit(0);
}
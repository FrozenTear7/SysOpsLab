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
int counter = 0;
sigset_t sigMask;

void sigintHandler(int signum) {
    exit(1);
}

void atexitHandler() {
    munmap(fifo, sizeof(fifo));
    sem_close(BARBER);
    sem_close(CLIENTS);
    sem_close(BLOCK);
}

int main(int argc, char **argv) {
    if (argc < 3)
        return 0;

    atexit(atexitHandler);
    signal(SIGINT, sigintHandler);

    int shmID = shm_open(pathShm, O_RDWR, 0666);
    fifo = (Fifo *) mmap(NULL, sizeof(Fifo), PROT_READ | PROT_WRITE, MAP_SHARED, shmID, 0);

    BARBER = sem_open(pathBarber, O_RDWR);
    CLIENTS = sem_open(pathFifo, O_RDWR);
    BLOCK = sem_open(pathBlock, O_RDWR);

    for (int i = 0; i < atoi(argv[1]); i++) {
        pid_t id = fork();

        if (id == 0) {
            while (counter < atoi(argv[2])) {

                sem_wait(BLOCK);

                int isEmpty = fifoEmpty(fifo);
                int res = fifoPush(fifo, getpid());

                if (res != -1) {
                    sem_post(CLIENTS);
                    sem_post(BLOCK);

                    if (isEmpty == 0) {
                        printf("%d sit in queue, Time: %ld\n", getpid(), timeMs());
                    } else {
                        printf("%d wake barber, Time: %ld\n", getpid(), timeMs());
                    }

                    sem_wait(BARBER);

                    printf("%d sit, Time: %ld\n", getpid(), timeMs());
                    printf("%d cut, Time: %ld\n", getpid(), timeMs());
                    counter++;
                } else {
                    printf("%d no free place, Time: %ld\n", getpid(), timeMs());

                    sem_post(BLOCK);

                    counter++;
                }
            }

            printf("%d leave, Time: %ld\n", getpid(), timeMs());

            exit(0);
        }
    }

    while (1) {
        wait(NULL);
        if (errno == ECHILD)
            break;
    }

    exit(0);
}
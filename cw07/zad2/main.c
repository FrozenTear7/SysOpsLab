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
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "info.h"

Fifo *fifo = NULL;
sigset_t sigMask;
int counter = 0;
sem_t *BARBER;
sem_t *FIFO;
sem_t *CHECKER;
sem_t *SLOWER;

void sigintHandler(int signum) {
    exit(1);
}

void sigrtminHandler(int signum) {
    counter++;
}

int takePlace() {
    int barberStat;
    sem_getvalue(BARBER, &barberStat);

    if (barberStat == 0) {
        sem_post(BARBER);
        printf("%d wakes barber up, Time: %ld\n", getpid(), timeMs());
        sem_wait(SLOWER);

        fifo->chair = getpid();

        return 1;
    } else {
        int res = fifoPush(fifo, getpid());

        if (res == -1) {
            printf("%d no free place, Time: %ld\n", getpid(), timeMs());
            return -1;
        } else {
            printf("%d in queue, Time: %ld\n", getpid(), timeMs());
            return 0;
        }
    }
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

    fifo = (Fifo *) mmap(NULL, sizeof(Fifo), PROT_READ | PROT_WRITE, MAP_SHARED, shm_open("/shm", O_RDWR, 0666), 0);

    BARBER = sem_open("/barber", O_CREAT | O_EXCL | O_RDWR, 0666, 0);
    FIFO = sem_open("/fifo", O_CREAT | O_EXCL | O_RDWR, 0666, 1);
    CHECKER = sem_open("/checker", O_CREAT | O_EXCL | O_RDWR, 0666, 1);
    SLOWER = sem_open("/slower", O_CREAT | O_EXCL | O_RDWR, 0666, 0);

    for (int i = 0; i < atoi(argv[1]); i++) {
        pid_t id = fork();

        if (id == 0) {
            while (counter < atoi(argv[2])) {
                sem_wait(CHECKER);
                sem_wait(FIFO);

                int res = takePlace();

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

    return 0;
}

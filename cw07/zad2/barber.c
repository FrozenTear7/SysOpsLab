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

key_t fifoKey;
Fifo *fifo = NULL;
sem_t *BARBER;
sem_t *FIFO;
sem_t *CHECKER;
sem_t *SLOWER;

void cut(pid_t pid) {
    printf("Start cutting %d, Time: %ld\n", pid, timeMs());

    kill(pid, SIGRTMIN);

    printf("End cutting %d, Time: %ld\n", pid, timeMs());
}

pid_t sitClient() {
    sem_wait(FIFO);

    pid_t nextClient = fifo->chair;

    printf("Sit %d, Time: %ld\n", nextClient, timeMs());

    sem_post(FIFO);

    return nextClient;
}

void work() {
    while (1) {
        sem_wait(BARBER);
        sem_post(BARBER);
        sem_post(SLOWER);

        printf("AWAKENING, Time: %ld\n", timeMs());
        pid_t nextClient = sitClient();
        cut(nextClient);

        while (1) {
            sem_wait(FIFO);

            nextClient = fifoPop(fifo);

            if (nextClient != -1) {
                printf("Sit %d, Time: %ld\n", nextClient, timeMs());

                sem_post(FIFO);

                cut(nextClient);
            } else {
                printf("SLEEP, Time: %ld\n", timeMs());

                sem_wait(BARBER);
                sem_post(FIFO);

                break;
            }
        }
    }
}

void atexitHandler(void) {
    munmap(fifo, sizeof(fifo));
    shm_unlink("/shm");
    sem_close(BARBER);
    sem_unlink("/barber");
    sem_close(FIFO);
    sem_unlink("/fifo");
    sem_close(CHECKER);
    sem_unlink("/checker");
    sem_close(SLOWER);
    sem_unlink("/slower");
}

void sigintHandler(int signum) {
    exit(1);
}

int main(int argc, char **argv) {
    if (argc < 2)
        return 0;

    atexit(atexitHandler);
    signal(SIGINT, sigintHandler);

    int shmID = shm_open("/shm", O_CREAT | O_EXCL | O_RDWR, 0666);
    ftruncate(shmID, sizeof(Fifo));
    fifo = (Fifo *) mmap(NULL, sizeof(Fifo), PROT_READ | PROT_WRITE, MAP_SHARED, shmID, 0);

    fifoInit(fifo, atoi(argv[1]));

    BARBER = sem_open("/barber", O_CREAT | O_EXCL | O_RDWR, 0666, 0);
    FIFO = sem_open("/fifo", O_CREAT | O_EXCL | O_RDWR, 0666, 1);
    CHECKER = sem_open("/checker", O_CREAT | O_EXCL | O_RDWR, 0666, 1);
    SLOWER = sem_open("/slower", O_CREAT | O_EXCL | O_RDWR, 0666, 0);

    work();

    return 0;
}

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
#include <sys/sem.h>
#include <sys/ipc.h>

#include "fifo.h"

key_t fifoKey;
Fifo *fifo = NULL;
int shmID = -1;
int semId = -1;

long getMicroTime() {
    struct timespec marker;
    clock_gettime(CLOCK_MONOTONIC, &marker);

    return marker.tv_nsec / 1000;
}

void fifoSet(int n) {
    fifoKey = ftok(getenv("HOME"), 5);

    shmID = shmget(fifoKey, sizeof(Fifo), IPC_CREAT | IPC_EXCL | 0666);

    void *tmp = shmat(shmID, NULL, 0);
    fifo = (Fifo *) tmp;

    fifoInit(fifo, n);
}

void semaphoreInit() {
    semId = semget(fifoKey, 4, IPC_CREAT | IPC_EXCL | 0666);

    for (int i = 1; i < 3; i++)
        semctl(semId, i, SETVAL, 1);

    semctl(semId, 0, SETVAL, 0);
}

void atexitClear() {
    shmdt(fifo);
    shmctl(shmID, IPC_RMID, NULL);
    semctl(semId, 0, IPC_RMID);
}

void intHandler(int signum) {
    exit(1);
}

void cut(pid_t pid) {
    printf("\"Barber: start cutting %d, Time: %ld\\n\"", pid, getMicroTime());
    kill(pid, SIGRTMIN);
    printf("Barber: end cutting %d, Time: %ld\n", pid, getMicroTime());
}

void work() {
    while (1) {
        struct sembuf sops;
        sops.sem_flg = 0;

        sops.sem_num = 0;
        sops.sem_op = -1;
        semop(semId, &sops, 1);

        sops.sem_num = 1;
        sops.sem_op = -1;
        semop(semId, &sops, 1);

        pid_t toCut = fifo->chair;

        sops.sem_op = 1;
        semop(semId, &sops, 1);

        while (1) {
            sops.sem_num = 1;
            sops.sem_op = -1;
            semop(semId, &sops, 1);

            toCut = fifoPop(fifo);

            if (toCut != -1) {
                sops.sem_op = 1;
                semop(semId, &sops, 1);
                cut(toCut);
            } else {
                printf("Barber: sleeping, Time: %ld\n", getMicroTime());
                fflush(stdout);

                sops.sem_num = 0;
                sops.sem_op = -1;
                semop(semId, &sops, 1);

                sops.sem_num = 1;
                sops.sem_op = 1;
                semop(semId, &sops, 1);
                break;
            }
        }
    }
}

int main(int argc, char **argv) {
    if (argc < 2)
        return 0;

    signal(SIGINT, intHandler);
    atexit(atexitClear);

    fifoSet(atoi(argv[1]));
    semaphoreInit();
    work();

    return 0;
}
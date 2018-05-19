#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

#include "info.h"

key_t fifoKey;
Fifo *fifo = NULL;
int semId = -1;
int shmId = -1;

void work() {
    fifo->asleep = 0;

    while (1) {
        struct sembuf sops;
        sops.sem_flg = 0;

        if (fifoEmpty(fifo)) {
            printf("SLEEP, Time: %ld\n", timeMs());
            fifo->asleep = 1;
        }

        sops.sem_num = CLIENTS;
        sops.sem_op = -1;
        semop(semId, &sops, 1);

        sops.sem_num = BLOCK;
        sops.sem_op = -1;
        semop(semId, &sops, 1);

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

            sops.sem_num = BARBER;
            sops.sem_op = 1;
            semop(semId, &sops, 1);

            sops.sem_num = BLOCK;
            sops.sem_op = 1;
            semop(semId, &sops, 1);

            printf("End cutting %d, Time: %ld\n", nextClient, timeMs());
        }
    }
}

void atexitHandler() {
    shmdt(fifo);
    shmctl(shmId, IPC_RMID, NULL);
    semctl(semId, 0, IPC_RMID);
}

void sigintHandler(int signum) {
    exit(1);
}

int main(int argc, char **argv) {
    if (argc < 2)
        return 0;

    atexit(atexitHandler);
    signal(SIGINT, sigintHandler);

    fifoKey = ftok(getenv("HOME"), keyId);
    shmId = shmget(fifoKey, sizeof(Fifo), IPC_CREAT | IPC_EXCL | 0666);
    fifo = (Fifo *) shmat(shmId, NULL, 0);

    fifoInit(fifo, atoi(argv[1]));

    semId = semget(fifoKey, 3, IPC_CREAT | IPC_EXCL | 0666);

    semctl(semId, BARBER, SETVAL, 0);
    semctl(semId, CLIENTS, SETVAL, 0);
    semctl(semId, BLOCK, SETVAL, 1);

    work();

    exit(0);
}
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

#include "info.h"

key_t fifoKey;
int shmID = -1;
Fifo *fifo = NULL;
int semId = -1;

void cut(pid_t pid) {
    printf("Time: %ld, Barber: preparing to cut %d\n", timeMs(), pid);

    kill(pid, SIGRTMIN);

    printf("Time: %ld, Barber: finished cutting %d\n", timeMs(), pid);
}

pid_t takeChair(struct sembuf *sops) {
    sops->sem_num = FIFO;
    sops->sem_op = -1;
    semop(semId, sops, 1);

    pid_t toCut = fifo->chair;

    sops->sem_op = 1;
    semop(semId, sops, 1);

    return toCut;
}

void napAndWorkForever() {
    while (1) {
        struct sembuf sops;
        sops.sem_num = BARBER;
        sops.sem_op = -1;
        sops.sem_flg = 0;

        semop(semId, &sops, 1);

        pid_t toCut = takeChair(&sops);
        cut(toCut);

        while (1) {
            sops.sem_num = FIFO;
            sops.sem_op = -1;
            semop(semId, &sops, 1);
            toCut = fifoPop(fifo);

            if (toCut != -1) {
                sops.sem_op = 1;
                semop(semId, &sops, 1);
                cut(toCut);
            } else {
                long timeMarker = timeMs();
                printf("Time: %ld, Barber: going to sleep...\n", timeMarker);
                sops.sem_num = BARBER;
                sops.sem_op = -1;
                semop(semId, &sops, 1);

                sops.sem_num = FIFO;
                sops.sem_op = 1;
                semop(semId, &sops, 1);
                break;
            }
        }
    }
}

void prepareFifo(int chNum) {
    fifoKey = ftok(getenv(env), keyId);
    shmID = shmget(fifoKey, sizeof(Fifo), IPC_CREAT | IPC_EXCL | 0666);
    fifo = (Fifo *) shmat(shmID, NULL, 0);

    fifoInit(fifo, chNum);
}

void prepareSemafors() {
    semId = semget(fifoKey, 4, IPC_CREAT | IPC_EXCL | 0666);

    for (int i = 1; i < 3; i++)
        semctl(semId, i, SETVAL, 1);

    semctl(semId, 0, SETVAL, 0);
}

void clearResources(void) {
    shmdt(fifo);
    shmctl(shmID, IPC_RMID, NULL);
    semctl(semId, 0, IPC_RMID);
}

void intHandler(int signo) {
    exit(2);
}

int main(int argc, char **argv) {
    if (argc < 2)
        return 0;

    atexit(clearResources);
    signal(SIGINT, intHandler);

    prepareFifo(atoi(argv[1]));
    prepareSemafors();
    napAndWorkForever();

    return 0;
}

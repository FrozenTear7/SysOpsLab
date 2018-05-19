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
int counter = 0;

void sigintHandler(int signum) {
    exit(1);
}

void atexitHandler() {
    shmdt(fifo);
}

int main(int argc, char **argv) {
    if (argc < 3)
        return 0;

    atexit(atexitHandler);
    signal(SIGINT, sigintHandler);

    fifoKey = ftok(getenv("HOME"), keyId);
    shmId = shmget(fifoKey, 0, 0);
    fifo = (Fifo *) shmat(shmId, NULL, 0);

    semId = semget(fifoKey, 0, 0);

    for (int i = 0; i < atoi(argv[1]); i++) {
        pid_t id = fork();

        if (id == 0) {
            while (counter < atoi(argv[2])) {
                struct sembuf sops;
                sops.sem_flg = 0;

                sops.sem_num = BLOCK;
                sops.sem_op = -1;
                semop(semId, &sops, 1);

                int isEmpty = fifoEmpty(fifo);
                int res = fifoPush(fifo, getpid());

                if (res != -1) {
                    sops.sem_num = CLIENTS;
                    sops.sem_op = 1;
                    semop(semId, &sops, 1);

                    sops.sem_num = BLOCK;
                    sops.sem_op = 1;
                    semop(semId, &sops, 1);

                    if (isEmpty == 0) {
                        printf("%d sit in queue, Time: %ld\n", getpid(), timeMs());
                    } else {
                        printf("%d wake barber, Time: %ld\n", getpid(), timeMs());
                    }

                    sops.sem_num = BARBER;
                    sops.sem_op = -1;
                    semop(semId, &sops, 1);

                    printf("%d sit, Time: %ld\n", getpid(), timeMs());
                    printf("%d cut, Time: %ld\n", getpid(), timeMs());
                    counter++;
                } else {
                    printf("%d no free place, Time: %ld\n", getpid(), timeMs());

                    sops.sem_num = BLOCK;
                    sops.sem_op = 1;
                    semop(semId, &sops, 1);

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

    return 0;
}
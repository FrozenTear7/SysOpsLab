#include <stdlib.h>
#include <stdio.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>

int main(int argc, char **argv) {
    key_t key = ftok("/main", 1);
    int semId = semget(key, 1, IPC_CREAT | 0666);
    struct sembuf *sops;
    sops->sem_num = 0;
    sops->sem_op = -1;
    sops->sem_flg = IPC_NOWAIT;

    pid_t child = fork();

    if (child == 0) {
        semop(semId, sops, 1);

        printf("%d\n", semctl(semId, 0, GETVAL));
    } else {
        sops->sem_op = 1;

        semop(semId, sops, 1);

        printf("%d\n", semctl(semId, 0, GETVAL));
    }

    semctl(semId, 1, IPC_RMID, NULL);

    return 0;
}
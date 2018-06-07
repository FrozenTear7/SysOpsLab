#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int main(int arc, char **argv) {
	int semId = semget(ftok(getenv("HOME"), 0), 1, IPC_CREAT | 0666);
	semctl(semId, 1, SETVAL, 1);

	pid_t child1 = fork();
	if(child1 == 0) {
		printf("1st child pid - %d\n", getpid());
		int shmId = shmget(ftok(getenv("HOME"), 1), sizeof(int), IPC_CREAT | 0666);
		int semId = semget(ftok(getenv("HOME"), 0), 0, 0);
		int *x = (int *) shmat(shmId, NULL, 0);

		struct sembuf sops;
		sops.sem_num = 1;
		sops.sem_op = -1;
		sops.sem_flg = 0;

		semop(semId, &sops, 1);

		*x = 2;
		printf("%d - %d\n", getpid(), *x);
		
		sops.sem_num = 1;
		sops.sem_op = 1;
		sops.sem_flg = 0;

		semop(semId, &sops, 1);

		exit(0);
	}

	pid_t child2 = fork();
	if(child2 == 0) {
		printf("2nd child pid - %d\n", getpid());
		int shmId = shmget(ftok(getenv("HOME"), 1), sizeof(int), IPC_CREAT | 0666);
		int semId = semget(ftok(getenv("HOME"), 0), 0, 0);
		int *x = (int *) shmat(shmId, NULL, 0);

		struct sembuf sops;
		sops.sem_num = 1;
		sops.sem_op = -1;
		sops.sem_flg = 0;

		semop(semId, &sops, 1);

		*x = 3;
		printf("%d - %d\n", getpid(), *x);

		sops.sem_num = 1;
		sops.sem_op = 1;
		sops.sem_flg = 0;

		semop(semId, &sops, 1);

		exit(0);
	}

	sleep(3);

	semctl(semId, 0, IPC_RMID);
	shmctl(semId, IPC_RMID, 0);	

	exit(0);
}

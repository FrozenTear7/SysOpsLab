#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <fcntl.h>
#include <signal.h>

int shmId;
sem_t *semId;
int *x;

void sigintHandler() {
    exit(0);
}

void atexitHandler() {
    munmap(x, sizeof(int));
    sem_close(semId);
}

int main(int arc, char **argv) {
    signal(SIGINT, sigintHandler);
    atexit(atexitHandler);

    semId = sem_open("/semEx2", O_RDWR);
    shmId = shm_open("/shmEx2", O_RDWR, 0666);
    ftruncate(shmId, sizeof(int));
    x = (int *) mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shmId, 0);

    sem_wait(semId);

    *x += 1;
    printf("%d\n", *x);

    sem_post(semId);

    while (1) {}
}

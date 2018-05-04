#ifndef INFO_H
#define INFO_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

//helpers

#define keyId 2
const char env[] = "HOME";

typedef enum semType {
    BARBER = 0, FIFO = 1, CHECKER = 2
} semType;

void throww(const char *err) {
    printf("Error! %s Errno: %d, %s\n", err, errno,
           strerror(errno)
    );
    exit(3);
}

long timeMs() {
    struct timespec timer;
    clock_gettime(CLOCK_MONOTONIC, &timer);

    return timer.tv_nsec / 1000;
}

//Fifo definition

typedef struct Fifo {
    int size;
    int head;
    int tail;
    pid_t chair;
    pid_t tab[1000];
} Fifo;

void fifoInit(Fifo *fifo, int n) {
    fifo->size = n;
    fifo->head = -1;
    fifo->tail = 0;
    fifo->chair = 0;
}

int fifoEmpty(Fifo *fifo) {
    if (fifo->head == -1)
        return 1;
    else
        return 0;
}

int fifoFull(Fifo *fifo) {
    if (fifo->head == fifo->tail)
        return 1;
    else
        return 0;
}

pid_t fifoPop(Fifo *fifo) {
    if (fifoEmpty(fifo) == 1)
        return -1;

    fifo->chair = fifo->tab[fifo->head++];

    if (fifo->head == fifo->size)
        fifo->head = 0;

    if (fifo->head == fifo->tail)
        fifo->head = -1;

    return fifo->chair;
}

int fifoPush(Fifo *fifo, pid_t x) {
    if (fifoFull(fifo) == 1)
        return -1;

    if (fifoEmpty(fifo) == 1) {
        fifo->head = fifo->tail;
    }

    fifo->tab[fifo->tail++] = x;

    if (fifo->tail == fifo->size)
        fifo->tail = 0;

    return 0;
}

#endif

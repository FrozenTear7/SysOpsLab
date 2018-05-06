#ifndef INFO_H
#define INFO_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//helpers

#define BARBER 0
#define CLIENTS 1
#define BLOCK 2

#define keyId 2

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
    pid_t tab[1000];
    pid_t chair;
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

int fifoPush(Fifo *fifo, pid_t pid) {
    if (fifoFull(fifo) == 1)
        return -1;

    if (fifoEmpty(fifo) == 1) {
        fifo->head = fifo->tail;
    }

    fifo->tab[fifo->tail++] = pid;

    if (fifo->tail == fifo->size)
        fifo->tail = 0;

    return 0;
}

#endif

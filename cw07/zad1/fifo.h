#ifndef FIFO_H
#define FIFO_H

typedef struct {
    pid_t buf[1000];
    int head;
    int tail;
    int size;
    pid_t chair;
} Fifo;

int fifoEmpty(Fifo *fifo) {
    if (fifo->head == -1)
        return 1;
    else
        return 0;
}

int fifoFull(Fifo *fifo) {
    if (fifo->tail == fifo->head)
        return 1;
    else
        return 0;
}

void fifoInit(Fifo *fifo, int n) {
    fifo->size = n;
    fifo->head = -1;
    fifo->tail = 0;
    fifo->chair = 0;
}

pid_t fifoPop(Fifo *fifo) {
    if (fifoEmpty(fifo) == 1)
        return -1;

    fifo->chair = fifo->buf[fifo->head++];

    if (fifo->head == fifo->size)
        fifo->head = 0;

    if (fifo->head == fifo->tail)
        return -1;

    return fifo->chair;
}

int fifoPush(Fifo *fifo, pid_t pid) {
    if (fifoFull(fifo) == 1)
        return -1;
    else if (fifoEmpty(fifo) == 1) {
        fifo->head = fifo->tail;
    }

    fifo->buf[fifo->tail++] = pid;

    if (fifo->tail == fifo->size)
        fifo->tail = 0;

    return 0;
}

#endif
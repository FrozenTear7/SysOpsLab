#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/times.h>
#include <semaphore.h>

sem_t mutex;
sem_t fillCount;
sem_t emptyCount;
int p, k, n, l, modeSearch, modePrint;
float nk;
char fileName[50];
char textFile[1024][1024];
int readIndex = 0, writeIndex = 0;
int fileCount = 0;
FILE *fp;
int endThreads = 0;
pthread_t *threadArrGlobal;
clock_t startTime;
clock_t endTime;
struct tms startCpu;
struct tms endCpu;
double timer[3];

void printArray() {
    puts("----");
    for (int i = 0; i < n; i++) {
        if (textFile[i])
            printf("%d: %s", i, textFile[i]);
    }
    puts("----");
}

long timeMs() {
    struct timespec timer;
    clock_gettime(CLOCK_MONOTONIC, &timer);

    return timer.tv_nsec / 1000;
}

void startClock() {
    startTime = times(&startCpu);
}

void endClock(double *result) {
    long clk = sysconf(_SC_CLK_TCK);
    endTime = times(&endCpu);
    result[0] = (endTime - startTime) / (double) clk;
    result[1] = (endCpu.tms_utime - startCpu.tms_utime) / (double) clk;
    result[2] = (endCpu.tms_stime - startCpu.tms_stime) / (double) clk;
}

void cancelThreads() {
    for (int i = 0; i < p + k; i++) {
        pthread_cancel(threadArrGlobal[i]);
    }

    fclose(fp);

    sem_destroy(&mutex);
    sem_destroy(&fillCount);
    sem_destroy(&emptyCount);

    exit(0);
}

void *producer(void *arg) {
    int i = *((int *) arg);
    free(arg);

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while (1) {
        sem_wait(&emptyCount);
        sem_wait(&mutex);

        if (endThreads || (nk != 0 && timer[0] > nk)) {
            sem_post(&mutex);
            sem_post(&fillCount);
            pthread_exit(NULL);
        }

        read = getline(&line, &len, fp);
        if (read == -1 || (nk != 0 && timer[0] > nk)) {
            endThreads = 1;
            sem_post(&mutex);
            sem_post(&fillCount);
            pthread_exit(NULL);
        }

        strcpy(textFile[writeIndex], line);

        if (textFile[writeIndex] && modePrint)
            printf("[Producer: %d]: index: %d: %s", i, writeIndex, textFile[writeIndex]);

        writeIndex++;
        if (writeIndex >= n)
            writeIndex = 0;

        fileCount++;

        sem_post(&mutex);
        sem_post(&fillCount);

        endClock(timer);
    }
}

void *consumer(void *arg) {
    int i = *((int *) arg);
    free(arg);

    while (1) {
        sem_wait(&fillCount);
        sem_wait(&mutex);

        if ((nk != 0 && timer[0] > nk) || (endThreads && fileCount == 0)) {
            sem_post(&mutex);
            sem_post(&emptyCount);
            sem_post(&fillCount);
            pthread_exit(NULL);
        }

        if (modePrint || (textFile[readIndex] && strlen(textFile[readIndex]) > l))
            printf("[Consumer: %d]: index: %d: %s", i, readIndex, textFile[readIndex]);

        readIndex++;
        if (readIndex >= n)
            readIndex = 0;

        fileCount--;

        sem_post(&mutex);
        sem_post(&emptyCount);

        endClock(timer);
    }
}

int main(int argc, char **argv) {
    if (argc != 2)
        exit(1);

    signal(SIGINT, cancelThreads);

    time_t t;
    srand((unsigned) time(&t));

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int lineCounter = 0;

    fp = fopen(argv[1], "r");
    if (fp == NULL)
        exit(1);

    while ((read = getline(&line, &len, fp)) != -1) {
        if (lineCounter == 0) {
            p = atoi(line);
        } else if (lineCounter == 1) {
            k = atoi(line);
        } else if (lineCounter == 2) {
            n = atoi(line);
        } else if (lineCounter == 3) {
            strncpy(fileName, line, strlen(line) - 1);
        } else if (lineCounter == 4) {
            l = atoi(line);
        } else if (lineCounter == 5) {
            modeSearch = atoi(line);
        } else if (lineCounter == 6) {
            modePrint = atoi(line);
        } else if (lineCounter == 7) {
            nk = atof(line);
        }

        lineCounter++;
    }

    fclose(fp);

    fp = fopen(fileName, "r");
    if (fp == NULL)
        exit(1);

    pthread_t *threadArr = (pthread_t *) malloc((p + k) * sizeof(pthread_t));
    threadArrGlobal = threadArr;

    startClock();

    sem_init(&mutex, 0, 1);
    sem_init(&emptyCount, 0, n);
    sem_init(&fillCount, 0, 0);

    for (int i = 0; i < p; i++) {
        int *threadArg = malloc(sizeof(*threadArg));
        *threadArg = i;
        pthread_create(&threadArr[i], NULL, producer, (void *) threadArg);
    }

    for (int i = p; i < p + k; i++) {
        int *threadArg = malloc(sizeof(*threadArg));
        *threadArg = i - p;
        pthread_create(&threadArr[i], NULL, consumer, (void *) threadArg);
    }

    for (int i = 0; i < (p + k); i++) {
        pthread_join(threadArr[i], NULL);
    }

    fclose(fp);

    sem_destroy(&mutex);
    sem_destroy(&fillCount);
    sem_destroy(&emptyCount);

    exit(0);
}

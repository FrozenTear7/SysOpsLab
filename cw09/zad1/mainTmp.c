#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/times.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
int p, k, n, l, modeSearch, modePrint, nk;
char fileName[50];
char textFile[1024][1024];
int readIndex = 0, writeIndex = 0;
int fileCount = 0;
FILE *fp;
int endConsumer = 0;
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

    exit(0);
}

void *producer(void *arg) {
    int i = *((int *) arg);
    free(arg);

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while (1) {
        pthread_mutex_lock(&mutex);
        //printf("LOCK producer %d, Time: %ld\n", i, timeMs());

        while (fileCount >= n) {
            //puts("Producer wait");
            pthread_cond_wait(&full, &mutex);
        }

        read = getline(&line, &len, fp);
        if (read == -1 || (nk != 0 && timer[0] > nk)) {
            endConsumer = 1;
            fileCount = 1;
            pthread_cond_broadcast(&empty);
            pthread_mutex_unlock(&mutex);
            pthread_exit(NULL);
        }

        //printArray();

        //textFile = malloc(strlen(line) * sizeof(char));
        strcpy(textFile[writeIndex], line);

        if (textFile[writeIndex] && modePrint)
            printf("[Producer: %d]: index: %d: %s", i, writeIndex, textFile[writeIndex]);

        //printArray();

        writeIndex++;
        if (writeIndex >= n)
            writeIndex = 0;

        fileCount++;
        if (fileCount == 1)
            pthread_cond_signal(&empty);

        pthread_mutex_unlock(&mutex);
        //printf("UNLOCK producer %d, Time: %ld\n", i, timeMs());

        //sleep(1);

        endClock(timer);
    }

    return NULL;
}

void *consumer(void *arg) {
    int i = *((int *) arg);
    free(arg);

    while (1) {
        if ((nk != 0 && timer[0] > nk) || endConsumer == 1) {
            pthread_exit(NULL);
        }

        pthread_mutex_lock(&mutex);
        //printf("LOCK consumer %d, Time: %ld\n", i, timeMs());

        while (fileCount <= 0) {
            //puts("Consumer wait");
            pthread_cond_wait(&empty, &mutex);
        }

        if ((nk != 0 && timer[0] > nk) || endConsumer == 1) {
            pthread_exit(NULL);
        }

        //printArray();

        if (modePrint || (textFile[readIndex] && strlen(textFile[readIndex]) > l))
            printf("[Consumer: %d]: index: %d: %s", i, readIndex, textFile[readIndex]);

        readIndex++;
        if (readIndex >= n)
            readIndex = 0;

        fileCount--;

        if (fileCount == n - 1)
            pthread_cond_signal(&full);

        pthread_mutex_unlock(&mutex);
        //printf("UNLOCK consumer %d, Time: %ld\n", i, timeMs());

        //sleep(1);

        endClock(timer);
    }

    return NULL;
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
            nk = atoi(line);
        }

        lineCounter++;
    }

    fclose(fp);

    fp = fopen(fileName, "r");
    if (fp == NULL)
        exit(1);

    pthread_t *threadArr = (pthread_t *) malloc((p + k) * sizeof(pthread_t));
    threadArrGlobal = threadArr;
    //textFile = malloc(n * sizeof(char *));

    startClock();

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

    exit(0);
}

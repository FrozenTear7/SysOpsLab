#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
int x = 1;
int p, k, n, l, modeSearch, modePrint, nk;
char fileName[50];
char **textFile;
int readIndex = 0, writeIndex = 0;
int fileCount = 0;
FILE *fp;
long fileOffset = 0;
int endConsumer = 0;
pthread_t *threadArrGlobal;

void cancelThreads() {
    for (int i = 0; i < p + k; i++) {
        pthread_cancel(threadArrGlobal[i]);
    }

    fclose(fp);

    exit(0);
}

void *producer(void *arg) {
    int i = *((int *) arg);
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    //printf("[Producer: %d]: Begin reading\n", i);

    while (1) {
        read = getline(&line, &len, fp);
        if (read == -1) {
            //endConsumer = 1;
            puts("ELO");
            pthread_exit(NULL);
        }

        pthread_mutex_lock(&mutex);

        if (fileCount >= n) {
            //printf("[Producer: %d]: WAIT, fileCount: %d\n", i, fileCount);
            pthread_cond_wait(&full, &mutex);
        }

        //textFile[writeIndex] = malloc(len * sizeof(char));
        textFile[writeIndex] = line;

        printf("[Producer: %d]: %s, fileCount: %d, writeIndex: %d\n", i, textFile[writeIndex], fileCount, writeIndex);
        if (!modeSearch && modePrint)
            printf("[Producer: %d]: %s", i);

        writeIndex++;
        if (writeIndex >= n)
            writeIndex = 0;

        fileCount++;

        pthread_mutex_unlock(&mutex);
        if (fileCount == 1)
            pthread_cond_signal(&empty);


        sleep(1);
    }

    return NULL;
}

void *consumer(void *arg) {
    int i = *((int *) arg);

    while (1) {
//        if (endConsumer == 1 && fileCount == 0) {
//            //printf("[Consumer: %d]: EXIT\n", i);
//            pthread_exit(NULL);
//        }

        pthread_mutex_lock(&mutex);

        if (fileCount <= 0) {
            //printf("[Consumer: %d]: WAIT, fileCount: %d\n", i, fileCount);
            pthread_cond_wait(&empty, &mutex);
        }

        printf("[Consumer: %d]: %s, fileCount: %d, readIndex: %d\n", i, textFile[readIndex], fileCount, readIndex);
        //printf("[Consumer: %d]: %s", i, textFile[readIndex]);

        readIndex++;
        if (readIndex >= n)
            readIndex = 0;

        //textFile[writeIndex] = null;
        fileCount--;

        pthread_mutex_unlock(&mutex);
        if (fileCount == n - 1)
            pthread_cond_signal(&full);


        sleep(1);
    }

    return NULL;
}

int main(int argc, char **argv) {
    if (argc != 2)
        exit(1);

    signal(SIGINT, cancelThreads);

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

    textFile = malloc(n * sizeof(char *));

    fp = fopen(fileName, "r");
    if (fp == NULL)
        exit(1);

    pthread_t *threadArr = (pthread_t *) malloc((p + k) * sizeof(pthread_t));
    threadArrGlobal = threadArr;
    textFile = malloc(n * sizeof(char *));

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

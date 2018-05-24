#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int x = 1;
int p, k, n, l, modeSearch, modePrint, nk;
char fileName[50];
char **textFile;
int readIndex = 0, writeIndex = 0;
FILE *fp;
long fileOffset = 0;

void *my_thread_safe_function() {
    pthread_mutex_lock(&mutex);
    printf("My id: %d, x: %d\n", (int) pthread_self(), x++);

    return NULL;
}

void *producer() {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while (1) {
        pthread_mutex_lock(&mutex);
        read = getline(&line, &len, fp);
        //textFile[writeIndex] = malloc(len * sizeof(char));
        textFile[writeIndex] = line;
        printf("%s", textFile[writeIndex]);
        pthread_mutex_unlock(&mutex);

        sleep(2);
    }

    return NULL;
}

void *consumer() {
    while (1) {
        //puts("xd1");
        sleep(3);
    }

    return NULL;
}

int main(int argc, char **argv) {
    if (argc != 2)
        exit(1);

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

    pthread_t *threadArr = malloc((p + k) * sizeof(pthread_t));
    textFile = malloc(n * sizeof(char *));

    for (int i = 0; i < k; i++) {
        pthread_create(&threadArr[i], NULL, producer, NULL);
    }

    for (int i = k; i < p + k; i++) {
        pthread_create(&threadArr[i], NULL, consumer, NULL);
    }

    for (int i = 0; i < (p + k); i++) {
        pthread_join(threadArr[i], NULL);
    }

    fclose(fp);

    exit(0);
}
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int x = 1;
int p, k, n, l, modeSearch, modePrint, nk;
char *fileName;


void *my_thread_safe_function() {
    pthread_mutex_lock(&mutex);
    printf("My id: %d, x: %d\n", (int) pthread_self(), x++);
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main(int argc, char **argv) {
    if (argc != 2)
        exit(1);

    int lineCounter = 0;

    fp = fopen(argv[2], "r");
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
            fileName = line;
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

    int noThreads = atoi(argv[1]);

    pthread_t *threadArr = malloc(noThreads * sizeof(pthread_t));

    for (int i = 0; i < noThreads; i++) {
        int *threadArg = malloc(sizeof(*threadArg));
        *threadArg = i;
        pthread_create(&threadArr[i], NULL, my_thread_safe_function, NULL);
    }

    for (int i = 0; i < noThreads; i++) {
        pthread_join(threadArr[i], NULL);
    }

    exit(0);
}
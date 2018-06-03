#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int x = 1;

void *my_thread_safe_function() {
    pthread_mutex_lock(&mutex);
    printf("My id: %d, x: %d\n", (int) pthread_self(), x++);
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main(int argc, char **argv) {
    if (argc != 2)
        exit(1);

    pthread_mutexattr_t attr;
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init (&mutex, &attr);

    int gettypeType;

    if(pthread_mutexattr_gettype(&attr, &gettypeType) == 0)
        printf("Mutex type: %d\n", gettypeType);

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
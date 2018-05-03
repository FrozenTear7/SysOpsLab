#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
    if (argc < 3)
        return 0;

    for (int i = 0; i < atoi(argv[1]); i++) {
        pid_t child = fork();

        if (child == 0) {
            //strzyzenie
            exit(0);
        }
    }

    return 0;
}
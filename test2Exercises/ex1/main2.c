#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
    pid_t parentPid = getpid();

    for (int i = 0; i < 5; i++) {
        if (getpid() == parentPid) {
            pid_t child = fork();

            if (child == 0) {
                printf("My pid: %d, parent pid: %d\n", getpid(), getppid());
            }
        }
    }

    exit(0);
}
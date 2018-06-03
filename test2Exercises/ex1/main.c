#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <string.h>

int serverKey;

typedef struct Msg {
    long mtype;
    char *mtext;
} Msg;

size_t msgSize = sizeof(Msg) - sizeof(long);

void atexitHandler() {
    msgctl(serverKey, IPC_RMID, 0);
}

void sigintHandler() {
    exit(1);
}

int main(int argc, char **argv) {
    signal(SIGINT, sigintHandler);
    atexit(atexitHandler);

    serverKey = msgget(ftok(getenv("HOME"), 1), IPC_CREAT | 0666);

    pid_t client = fork();

    if (client == 0) {
        Msg msg;
        msg.mtype = 1;
        msg.mtext = "xd";

        msgsnd(serverKey, &msg, msgSize, 0);
    } else if (client > 0) {
        Msg msg;

        msgrcv(serverKey, &msg, msgSize, 0, 0);

        printf("Received: %s\n", msg.mtext);
    }

    exit(0);
}
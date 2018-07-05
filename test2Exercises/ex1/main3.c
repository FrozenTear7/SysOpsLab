#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <signal.h>
#include <string.h>

mqd_t serverKey;

typedef struct Msg {
    long mtype;
    char mtext[50];
} Msg;

void atexitHandler() {
    mq_close(serverKey);
    mq_unlink("/server");
}

void sigintHandler() {
    exit(1);
}

int main(int argc, char **argv) {
    signal(SIGINT, sigintHandler);
    atexit(atexitHandler);

    struct mq_attr posixAttr;
    posixAttr.mq_maxmsg = 9;
    posixAttr.mq_msgsize = 50;

    serverKey = mq_open("/server", O_RDWR | O_CREAT, 0666, &posixAttr);

    pid_t client = fork();

    if (client == 0) {
        Msg msg;
        msg.mtype = 1;
        sprintf(msg.mtext, "%s", "xd");

        if (mq_send(serverKey, (char *) &msg, sizeof(msg.mtext), 1) == -1) {
            perror("Send Error");
            exit(0);
        }
    } else if (client > 0) {
        Msg msg;

        if (mq_receive(serverKey, (char *) &msg, sizeof(msg.mtext), NULL) == -1) {
            perror("Receive Error");
            exit(0);
        }

        printf("Received: %s\n", msg.mtext);
    }

    exit(0);
}
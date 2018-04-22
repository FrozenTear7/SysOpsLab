#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <mqueue.h>
#include <ctype.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <mqueue.h>

#include "info.h"

int sessionID = -1;
mqd_t publicID = -1;
mqd_t privateID = -1;
char myPath[20];

char **arguments(char *line);

void loginClient();

void requestMirror(struct Msg *msg, char *str);

void requestCalc(struct Msg *msg, char *a, char *b, mtype requestType);

void requestTime(struct Msg *msg);

void requestEnd(struct Msg *msg);

void requestQueue(struct Msg *msg);

void deleteQueue();

void sigintHandler(int signum);

int main(int argc, char **argv) {
    if (argc < 2)
        return 1;

    FILE *fp;
    char *line, *path;
    key_t keyPrivate;
    size_t len = 0;
    int read;

    fp = fopen(argv[1], "r");
    if (fp == NULL)
        return 1;

    atexit(deleteQueue);
    signal(SIGINT, sigintHandler);
    sprintf(myPath, "/%d", getpid());

    if ((publicID = mq_open(serverPath, O_WRONLY)) == -1) {
        puts("Couldnt open public queue");
        return 1;
    }

    struct mq_attr posixAttr;
    posixAttr.mq_maxmsg = MAX_MQSIZE;
    posixAttr.mq_msgsize = MSG_SIZE;

    if ((privateID = mq_open(myPath, O_RDONLY | O_CREAT | O_EXCL, 0666, &posixAttr)) == -1) {
        puts("Couldnt create private queues");
        return 1;
    }

    loginClient();

    while ((read = getline(&line, &len, fp)) != -1) {
        char **requestArgv = arguments(line);
        Msg msg;
        msg.senderPID = getpid();

        if (strcmp(requestArgv[0], "mirror") == 0) {
            requestMirror(&msg, requestArgv[1]);
        } else if (strcmp(requestArgv[0], "add") == 0) {
            requestCalc(&msg, requestArgv[1], requestArgv[2], ADD);
        } else if (strcmp(requestArgv[0], "mul") == 0) {
            requestCalc(&msg, requestArgv[1], requestArgv[2], MUL);
        } else if (strcmp(requestArgv[0], "sub") == 0) {
            requestCalc(&msg, requestArgv[1], requestArgv[2], SUB);
        } else if (strcmp(requestArgv[0], "div") == 0) {
            requestCalc(&msg, requestArgv[1], requestArgv[2], DIV);
        } else if (strcmp(requestArgv[0], "time") == 0) {
            requestTime(&msg);
        } else if (strcmp(requestArgv[0], "end") == 0) {
            requestEnd(&msg);
        } else
            puts("Wrong argument");
    }

    fclose(fp);

    return 0;
}

char **arguments(char *line) {
    int argc = 0;
    char **argv = malloc(3 * sizeof(char *));

    char *current_arg = NULL;
    while ((current_arg = strtok(current_arg == NULL ? line : NULL, " \n")) != NULL && argc < 3) {
        argv[argc++] = current_arg;
    }

    return argv;
}

void loginClient() {
    Msg msg;
    msg.mtype = LOGIN;
    msg.senderPID = getpid();

    if (mq_send(publicID, (char *) &msg, MSG_SIZE, 1) == -1) {
        puts("Login request error");
        exit(0);
    }

    if (mq_receive(privateID, (char *) &msg, MSG_SIZE, NULL) == -1) {
        puts("Couldnt receive login response");
        exit(0);
    }

    if (sscanf(msg.mtext, "%d", &sessionID) < 1) {
        puts("Couldnt read login response");
        exit(0);
    }

    if (sessionID < 0) {
        puts("Client limit");
        exit(0);
    }

    printf("Logged in: %d!\n", sessionID);
}

void requestMirror(struct Msg *msg, char *str) {
    msg->mtype = MIRROR;
    if (strlen(str) > MAX_MTEXT_SIZE) {
        puts("Too many characters");
        exit(0);
    }

    strcpy(msg->mtext, str);

    if (mq_send(publicID, (char *) msg, MSG_SIZE, 1) == -1) {
        puts("Mirror request error");
        exit(0);
    }

    if (mq_receive(privateID, (char *) msg, MSG_SIZE, NULL) == -1) {
        puts("Mirror response error");
        exit(0);
    }

    puts(msg->mtext);
}

void requestCalc(struct Msg *msg, char *a, char *b, mtype requestType) {
    msg->mtype = requestType;
    char buf[MAX_MTEXT_SIZE];
    strcpy(buf, a);
    strcat(buf, " ");
    strcat(buf, b);

    if (strlen(buf) > MAX_MTEXT_SIZE) {
        puts("Too many characters");
        exit(0);
    }

    strcpy(msg->mtext, buf);

    if (mq_send(publicID, (char *) msg, MSG_SIZE, 1) == -1) {
        puts("Calc request error");
        exit(0);
    }

    if (mq_receive(privateID, (char *) msg, MSG_SIZE, NULL) == -1) {
        puts("Calc response error");
        exit(0);
    }

    puts(msg->mtext);
}

void requestTime(struct Msg *msg) {
    msg->mtype = TIME;

    if (mq_send(publicID, (char *) msg, MSG_SIZE, 1) == -1) {
        puts("Time request error");
        exit(0);
    }

    if (mq_receive(privateID, (char *) msg, MSG_SIZE, NULL) == -1) {
        puts("Time response error");
        exit(0);
    }

    puts(msg->mtext);
}

void requestEnd(struct Msg *msg) {
    msg->mtype = END;

    if (mq_send(publicID, (char *) msg, MSG_SIZE, 1) == -1) {
        puts("End request error");
        exit(0);
    }
}

void requestQueue(struct Msg *msg) {
    msg->mtype = QUIT;

    if (mq_send(publicID, (char *) msg, MSG_SIZE, 1) == -1)
        puts("Quit request error");
}

void deleteQueue() {
    if (privateID != -1) {
        if (sessionID >= 0) {
            puts("Ask server to close queue");
            Msg msg;
            msg.senderPID = getpid();
            requestQueue(&msg);
        }

        if (mq_close(publicID) == -1)
            puts("Couldnt not close server's queue");
        else
            puts("Server's queue closed");

        if (mq_close(privateID) == -1)
            puts("Couldnt close queue");
        else
            puts("Queue closed");

        if (mq_unlink(myPath) == -1)
            puts("Couldnt delete queue");
        else
            puts("Queue deleted");
    } else
        puts("No queue to delete");
}

void sigintHandler(int signum) {
    exit(0);
}
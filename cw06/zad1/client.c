#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>

#include "info.h"

int sessionID = -1;
int publicID = -1;
int privateID = -1;

char **arguments(char *line);

void loginClient(key_t keyPrivate);

void requestMirror(struct Msg *msg, char *str);

void requestCalc(struct Msg *msg, char *a, char *b, mtype requestType);

void requestTime(struct Msg *msg);

void requestEnd(struct Msg *msg);

void deleteQueue();

void sigintHandler(int signum);

int getQueueId(char *path, int ID);

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
    path = getenv("HOME");
    publicID = getQueueId(path, PROJECT_ID);
    keyPrivate = ftok(path, getpid());

    if ((privateID = msgget(keyPrivate, IPC_CREAT | IPC_EXCL | 0666)) == -1) {
        puts("Msgget error");
        return 1;
    }

    loginClient(keyPrivate);

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

void loginClient(key_t keyPrivate) {
    Msg msg;
    msg.mtype = LOGIN;
    msg.senderPID = getpid();
    sprintf(msg.mtext, "%d", keyPrivate);

    if (msgsnd(publicID, &msg, MSG_SIZE, 0) == -1) {
        puts("Login request error");
        exit(0);
    }

    if (msgrcv(privateID, &msg, MSG_SIZE, 0, 0) == -1) {
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

    if (msgsnd(publicID, msg, MSG_SIZE, 0) == -1) {
        puts("Mirror request error");
        exit(0);
    }

    if (msgrcv(privateID, msg, MSG_SIZE, 0, 0) == -1) {
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

    if (msgsnd(publicID, msg, MSG_SIZE, 0) == -1) {
        puts("Calc request error");
        exit(0);
    }

    if (msgrcv(privateID, msg, MSG_SIZE, 0, 0) == -1) {
        puts("Calc response error");
        exit(0);
    }

    puts(msg->mtext);
}

void requestTime(struct Msg *msg) {
    msg->mtype = TIME;

    if (msgsnd(publicID, msg, MSG_SIZE, 0) == -1) {
        puts("Time request error");
        exit(0);
    }

    if (msgrcv(privateID, msg, MSG_SIZE, 0, 0) == -1) {
        puts("Time response error");
        exit(0);
    }

    puts(msg->mtext);
}

void requestEnd(struct Msg *msg) {
    msg->mtype = END;

    if (msgsnd(publicID, msg, MSG_SIZE, 0) == -1) {
        puts("End request error");
        exit(0);
    }
}

void requestQueue(struct Msg *msg) {
    msg->mtype = QUIT;

    if (msgsnd(publicID, (char *) msg, MSG_SIZE, 0) == -1)
        puts("Quit request error");
}

void deleteQueue() {
    if (privateID != -1) {
        msgctl(privateID, IPC_RMID, NULL);
        puts("Queue deleted");
        Msg msg;
        msg.senderPID = getpid();
        requestQueue(&msg);
    }
}

void sigintHandler(int signum) {
    exit(0);
}

int getQueueId(char *path, int ID) {
    int key, queueId;
    key = ftok(path, ID);

    if ((queueId = msgget(key, 0)) == -1) {
        puts("Couldnt open the queue");
        exit(0);
    }

    return queueId;
}
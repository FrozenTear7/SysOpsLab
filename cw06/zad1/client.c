#define _GNU_SOURCE

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

#define MAX_CLIENTS  10
#define PROJECT_ID 37
#define MAX_CONT_SIZE 50

int getQID(char *path, int ID) {
    int key = ftok(path, ID);
    if (key == -1) {
        puts("Ftok error");
        exit(1);
    }

    int QID = msgget(key, 0);
    if (QID == -1) {
        puts("Couldnt open the queue");
        exit(1);
    }

    return QID;
}

typedef enum mtype {
    LOGIN = 1, MIRROR = 2, ADD = 3, MUL = 4, SUB = 5, DIV = 6, TIME = 7, END = 8, INIT = 9
} mtype;

typedef struct Msg {
    long mtype;
    pid_t senderPID;
    char cont[MAX_CONT_SIZE];
} Msg;

const size_t MSG_SIZE = sizeof(Msg) - sizeof(long);

char **arguments(char *line);

void loginClient(key_t keyPrivate);

void rqMirror(struct Msg *msg, char *str);

void rqCalc(struct Msg *msg, char *a, char *b, mtype rqType);

void rqTime(struct Msg *msg);

void rqEnd(struct Msg *msg);

int sessionID = -2;
int publicID = -1;
int privateID = -1;

void rmQueue(void) {
    if (privateID > -1) {
        if (msgctl(privateID, IPC_RMID, NULL) == -1) {
            puts("Queue removal error");
        } else
            puts("Queue deleted");
    }
}

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

    if (atexit(rmQueue) == -1) {
        puts("Atexit error");
        return 1;
    }

    if ((path = getenv("HOME")) == NULL) {
        puts("Couldnt get home env");
        return 1;
    }

    publicID = getQID(path, PROJECT_ID);

    if ((keyPrivate = ftok(path, getpid())) == -1) {
        puts("Couldnt get private key");
        return 1;
    }

    if ((privateID = msgget(keyPrivate, IPC_CREAT | IPC_EXCL | 0666)) == -1) {
        printf("Errno: %d\n", errno);
        return 1;
    }

    loginClient(keyPrivate);

    while ((read = getline(&line, &len, fp)) != -1) {
        //printf("%s\n", line);
        char **rqArgv = arguments(line);
        Msg msg;
        msg.senderPID = getpid();
/*
        for(int i = 0; i < 3; i++)
            if(rqArgv[i])
                printf("%s\n", rqArgv[i]);*/

        if (strcmp(rqArgv[0], "mirror") == 0) {
            rqMirror(&msg, rqArgv[1]);
        } else if (strcmp(rqArgv[0], "add") == 0) {
            rqCalc(&msg, rqArgv[1], rqArgv[2], ADD);
        } else if (strcmp(rqArgv[0], "mul") == 0) {
            rqCalc(&msg, rqArgv[1], rqArgv[2], MUL);
        } else if (strcmp(rqArgv[0], "sub") == 0) {
            rqCalc(&msg, rqArgv[1], rqArgv[2], SUB);
        } else if (strcmp(rqArgv[0], "div") == 0) {
            rqCalc(&msg, rqArgv[1], rqArgv[2], DIV);
        } else if (strcmp(rqArgv[0], "time") == 0) {
            rqTime(&msg);
        } else if (strcmp(rqArgv[0], "end") == 0) {
            rqEnd(&msg);
        } else if (strcmp(rqArgv[0], "q") == 0) {
            exit(0);
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
    sprintf(msg.cont, "%d", keyPrivate);

    if (msgsnd(publicID, &msg, MSG_SIZE, 0) == -1) {
        puts("Login request error");
        exit(1);
    }
    if (msgrcv(privateID, &msg, MSG_SIZE, 0, 0) == -1) {
        puts("Couldnt receive login response");
        exit(1);
    }
    if (sscanf(msg.cont, "%d", &sessionID) < 1) {
        puts("Couldnt read login response");
        exit(1);
    }
    if (sessionID < 0) {
        puts("Client limit");
        exit(1);
    }

    printf("Logged in: %d!\n", sessionID);
}

void rqMirror(struct Msg *msg, char *str) {
    msg->mtype = MIRROR;
    if (strlen(str) > MAX_CONT_SIZE) {
        printf("Too many characters!\n");
        exit(1);
    }
    strcpy(msg->cont, str);
    if (msgsnd(publicID, msg, MSG_SIZE, 0) == -1) {
        puts("Mirror request error");
        exit(1);
    }
    if (msgrcv(privateID, msg, MSG_SIZE, 0, 0) == -1) {
        puts("Mirror response error");
        exit(1);
    }
    printf("%s", msg->cont);
}

void rqCalc(struct Msg *msg, char *a, char *b, mtype rqType) {
    msg->mtype = rqType;
    char buf[MAX_CONT_SIZE];
    strcpy(buf, a);
    strcat(buf, " ");
    strcat(buf, b);
    if (strlen(buf) > MAX_CONT_SIZE) {
        printf("Too many characters!\n");
        exit(1);
    }
    strcpy(msg->cont, buf);
    if (msgsnd(publicID, msg, MSG_SIZE, 0) == -1) {
        puts("Calc request error");
        exit(1);
    }
    if (msgrcv(privateID, msg, MSG_SIZE, 0, 0) == -1) {
        puts("Calc response error");
        exit(1);
    }
    printf("%s", msg->cont);
}

void rqTime(struct Msg *msg) {
    msg->mtype = TIME;

    if (msgsnd(publicID, msg, MSG_SIZE, 0) == -1) {
        puts("Time request error");
        exit(1);
    }
    if (msgrcv(privateID, msg, MSG_SIZE, 0, 0) == -1) {
        puts("Time response error");
        exit(1);
    }
    printf("%s\n", msg->cont);
}

void rqEnd(struct Msg *msg) {
    msg->mtype = END;

    if (msgsnd(publicID, msg, MSG_SIZE, 0) == -1) {
        puts("End request error");
        exit(1);
    }
}
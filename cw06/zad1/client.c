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

char **arguments(char *line) {
    int argc = 0;
    char **argv = malloc(3 * sizeof(char *));

    char *current_arg = NULL;
    while ((current_arg = strtok(current_arg == NULL ? line : NULL, " \n")) != NULL && argc < 3) {
        argv[argc++] = current_arg;
    }

    return argv;
}

void throww(const char *err) {
    printf("Error! %s Errno: %d\n", err, errno);
    exit(3);
}

int getQID(char *path, int ID) {
    int key = ftok(path, ID);
    if (key == -1) throww("Generation of key failed!");

    int QID = msgget(key, 0);
    if (QID == -1) throww("Opening queue failed!");

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

void registerClient(key_t privateKey);

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
            printf("There was some error deleting clients's queue!\n");
        } else printf("Client's queue deleted successfully!\n");
    }
}

void intHandler(int signo) {
    exit(2);
}

int main(int argc, char **argv) {
    if (argc < 2)
        return 1;

    FILE *fp;
    char *line;
    size_t len = 0;
    int read;

    fp = fopen(argv[1], "r");
    if (fp == NULL)
        return 1;

    if (atexit(rmQueue) == -1) throww("Registering client's atexit failed!");
    if (signal(SIGINT, intHandler) == SIG_ERR) throww("Registering INT failed!");

    char *path = getenv("HOME");
    if (path == NULL) throww("Getting enviromental variable 'HOME' failed!");

    publicID = getQID(path, PROJECT_ID);

    key_t privateKey = ftok(path, getpid());
    if (privateKey == -1) throww("Generation of private key failed!");

    privateID = msgget(privateKey, IPC_CREAT | IPC_EXCL | 0666);
    if (privateID == -1) throww("Creation of private queue failed!");

    registerClient(privateKey);

    while ((read = getline(&line, &len, fp)) != -1) {
        printf("%s\n", line);
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
        } else printf("Wrong command!\n");
    }

    fclose(fp);

    return 0;
}

void registerClient(key_t privateKey) {
    Msg msg;
    msg.mtype = LOGIN;
    msg.senderPID = getpid();
    sprintf(msg.cont, "%d", privateKey);

    if (msgsnd(publicID, &msg, MSG_SIZE, 0) == -1) throww("LOGIN request failed!");
    if (msgrcv(privateID, &msg, MSG_SIZE, 0, 0) == -1) throww("catching LOGIN response failed!");
    if (sscanf(msg.cont, "%d", &sessionID) < 1) throww("scanning LOGIN response failed!");
    if (sessionID < 0) throww("Server cannot have more clients!");

    printf("Client registered! My session nr is %d!\n", sessionID);
}

void rqMirror(struct Msg *msg, char *str) {
    msg->mtype = MIRROR;
    if (strlen(str) > MAX_CONT_SIZE) {
        printf("Too many characters!\n");
        return;
    }
    strcpy(msg->cont, str);
    if (msgsnd(publicID, msg, MSG_SIZE, 0) == -1) throww("MIRROR request failed!");
    if (msgrcv(privateID, msg, MSG_SIZE, 0, 0) == -1) throww("catching MIRROR response failed!");
    printf("%s", msg->cont);
}

void rqCalc(struct Msg *msg, char *a, char *b, mtype rqType) {
    msg->mtype = rqType;
    if (fgets(msg->cont, MAX_CONT_SIZE, stdin) == NULL) {
        printf("Too many characters!\n");
        return;
    }
    strcpy(msg->cont, a);
    strcpy(msg->cont, " ");
    strcpy(msg->cont, b);
    if (msgsnd(publicID, msg, MSG_SIZE, 0) == -1) throww("UPPER request failed!");
    if (msgrcv(privateID, msg, MSG_SIZE, 0, 0) == -1) throww("catching UPPER response failed!");
    printf("%s", msg->cont);
}

void rqTime(struct Msg *msg) {
    msg->mtype = TIME;

    if (msgsnd(publicID, msg, MSG_SIZE, 0) == -1) throww("TIME request failed!");
    if (msgrcv(privateID, msg, MSG_SIZE, 0, 0) == -1) throww("catching TIME response failed!");
    printf("%s\n", msg->cont);
}

void rqEnd(struct Msg *msg) {
    msg->mtype = END;

    if (msgsnd(publicID, msg, MSG_SIZE, 0) == -1) throww("END request failed!");
}
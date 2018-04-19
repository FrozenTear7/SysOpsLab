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
    char **argv = malloc(2 * sizeof(char *));

    char *current_arg = NULL;
    while ((current_arg = strtok(current_arg == NULL ? line : NULL, " \n")) != NULL && argc < 2) {
        argv[argc++] = current_arg;
    }

    return argv;
}

void reverse(char *str) {
    if (str == 0) {
        return;
    }

    if (*str == 0) {
        return;
    }

    char *start = str;
    char *end = start + strlen(str) - 1;
    char temp;

    while (end > start) {
        temp = *start;
        *start = *end;
        *end = temp;
        ++start;
        --end;
    }
}

char *convertTime(const time_t *mtime) {
    char *buff = malloc(sizeof(char) * 30);
    struct tm *timeinfo;
    timeinfo = localtime(mtime);
    strftime(buff, 20, "%b %d %H:%M", timeinfo);
    return buff;
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

void publicQueueExecute(struct Msg *msg);

void doLogin(struct Msg *msg);

void doMirror(struct Msg *msg);

void doCalc(struct Msg *msg, mtype rqType);

void doTime(struct Msg *msg);

void doEnd(struct Msg *msg);

int findQID(pid_t senderPID);

int prepareMsg(struct Msg *msg);

int publicID = -2;
int active = 1;
int clientsData[MAX_CLIENTS][2];
int clientCnt = 0;

void rmQueue(void) {
    if (publicID > -1) {
        int tmp = msgctl(publicID, IPC_RMID, NULL);
        if (tmp == -1) {
            printf("There was some error deleting server's queue!\n");
        }
        printf("Server's queue deleted successfully!\n");
    }
}

void intHandler(int signo) {
    exit(2);
}

int main(int argc, char **argv) {
    if (atexit(rmQueue) == -1) throww("Registering server's atexit failed!");
    if (signal(SIGINT, intHandler) == SIG_ERR) throww("Registering INT failed!");

    struct msqid_ds currentState;

    char *path = getenv("HOME");
    if (path == NULL) throww("Getting enviromental variable 'HOME' failed!");

    key_t publicKey = ftok(path, PROJECT_ID);
    if (publicKey == -1) throww("Generation of publicKey failed!");

    publicID = msgget(publicKey, IPC_CREAT | IPC_EXCL | 0666);
    if (publicID == -1) throww("Creation of public queue failed!");

    Msg buff;
    while (1) {
        if (active == 0) {
            if (msgctl(publicID, IPC_STAT, &currentState) == -1)
                throww("Getting current state of public queue failed!\n");
            if (currentState.msg_qnum == 0) break;
        }

        if (msgrcv(publicID, &buff, MSG_SIZE, 0, 0) < 0) throww("Receiving message failed!");
        publicQueueExecute(&buff);
    }
    return 0;
}

void publicQueueExecute(struct Msg *msg) {
    if (msg == NULL) return;
    switch (msg->mtype) {
        case LOGIN:
            doLogin(msg);
            break;
        case MIRROR:
            doMirror(msg);
            break;
        case ADD:
        case MUL:
        case SUB:
        case DIV:
            doCalc(msg, msg->mtype);
            break;
        case TIME:
            doTime(msg);
            break;
        case END:
            doEnd(msg);
            break;
        default:
            break;
    }
}

void doLogin(struct Msg *msg) {
    key_t clientQKey;
    if (sscanf(msg->cont, "%d", &clientQKey) < 0) throww("Reading clientKey failed!");

    int clientQID = msgget(clientQKey, 0);
    if (clientQID == -1) throww("Reading clientQID failed!");

    int clientPID = msg->senderPID;
    msg->mtype = INIT;
    msg->senderPID = getpid();

    if (clientCnt > MAX_CLIENTS - 1) {
        printf("Maximum amount of clients reached!\n");
        sprintf(msg->cont, "%d", -1);
    } else {
        clientsData[clientCnt][0] = clientPID;
        clientsData[clientCnt++][1] = clientQID;
        sprintf(msg->cont, "%d", clientCnt - 1);
    }

    if (msgsnd(clientQID, msg, MSG_SIZE, 0) == -1) throww("LOGIN response failed!");
}

void doMirror(struct Msg *msg) {
    int clientQID = prepareMsg(msg);
    if (clientQID == -1) return;

    reverse(msg->cont);

    if (msgsnd(clientQID, msg, MSG_SIZE, 0) == -1) throww("MIRROR response failed!");
}

void doCalc(struct Msg *msg, mtype rqType) {
    int clientQID = prepareMsg(msg);
    if (clientQID == -1) return;

    char **rqArgv = arguments(msg->cont);
    int result;

    switch (rqType) {
        case ADD: {
            result = atoi(rqArgv[0]) + atoi(rqArgv[1]);
            sprintf(msg->cont, "%d", result);
        }
            break;
        case MUL:
            result = atoi(rqArgv[0]) * atoi(rqArgv[1]);
            sprintf(msg->cont, "%d", result);
            break;
        case SUB:
            result = atoi(rqArgv[0]) - atoi(rqArgv[1]);
            sprintf(msg->cont, "%d", result);
            break;
        case DIV:
            result = atoi(rqArgv[0]) / atoi(rqArgv[1]);
            sprintf(msg->cont, "%d", result);
            break;
    }

    if (msgsnd(clientQID, msg, MSG_SIZE, 0) == -1) throww("CALC response failed!");
}

void doTime(struct Msg *msg) {
    int clientQID = prepareMsg(msg);
    if (clientQID == -1) return;

    time_t timer;
    time(&timer);
    char *timeStr = convertTime(&timer);

    sprintf(msg->cont, "%s", timeStr);
    free(timeStr);

    if (msgsnd(clientQID, msg, MSG_SIZE, 0) == -1) throww("TIME response failed!");
}

void doEnd(struct Msg *msg) {
    active = 0;
}

int prepareMsg(struct Msg *msg) {
    int clientQID = findQID(msg->senderPID);
    if (clientQID == -1) {
        printf("Client Not Found!\n");
        return -1;
    }

    msg->mtype = msg->senderPID;
    msg->senderPID = getpid();

    return clientQID;
}

int findQID(pid_t senderPID) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clientsData[i][0] == senderPID) return clientsData[i][1];
    }
    return -1;
}
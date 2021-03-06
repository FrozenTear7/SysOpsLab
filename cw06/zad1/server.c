#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <ctype.h>
#include <signal.h>

#include "info.h"

int publicID = -1;
int active = 1;
int clientsData[MAX_CLIENTS][2];
int clientCount = 0;

void execRequest(struct Msg *msg);

void execLogin(struct Msg *msg);

void execMirror(struct Msg *msg);

void execCalc(struct Msg *msg, mtype rqType);

void execTime(struct Msg *msg);

void execEnd(struct Msg *msg);

void execQuit(struct Msg *msg);

int findQueueID(pid_t senderPID);

int prepareMsg(struct Msg *msg);

char **arguments(char *line);

void reverse(char *str);

void deleteQueue();

void intHandler(int signum);

int main(int argc, char **argv) {
    key_t publicKey;
    char *path;
    struct msqid_ds currentState;
    Msg buff;

    atexit(deleteQueue);
    signal(SIGINT, intHandler);
    path = getenv("HOME");
    publicKey = ftok(path, PROJECT_ID);

    if ((publicID = msgget(publicKey, IPC_CREAT | IPC_EXCL | 0666)) == -1) {
        puts("Couldnt create public queue");
        return 1;
    }

    puts("Server waiting for requests");

    while (1) {
        if (active == 0) {
            msgctl(publicID, IPC_STAT, &currentState);
            if (currentState.msg_qnum == 0)
                break;
        }

        if (msgrcv(publicID, &buff, MSG_SIZE, 0, 0) < 0) {
            puts("Couldnt receive message");
            return 1;
        }

        execRequest(&buff);
    }

    return 0;
}

void execRequest(struct Msg *msg) {
    if (msg == NULL)
        return;

    switch (msg->mtype) {
        case LOGIN:
            execLogin(msg);
            break;
        case MIRROR:
            execMirror(msg);
            break;
        case ADD:
        case MUL:
        case SUB:
        case DIV:
            execCalc(msg, msg->mtype);
            break;
        case TIME:
            execTime(msg);
            break;
        case END:
            execEnd(msg);
            break;
        case QUIT:
            execQuit(msg);
            break;
        default:
            break;
    }
}

void execLogin(struct Msg *msg) {
    key_t clientQueueKey;
    int clientQueueId;
    if (sscanf(msg->mtext, "%d", &clientQueueKey) < 0) {
        puts("Couldnt read client key");
        return;
    }

    if ((clientQueueId = msgget(clientQueueKey, 0)) == -1) {
        puts("Couldnt read clientID");
        return;
    }

    int clientPID = msg->senderPID;
    msg->mtype = INIT;
    msg->senderPID = getpid();

    if (clientCount > MAX_CLIENTS - 1) {
        puts("Clients limit");
        sprintf(msg->mtext, "%d", -1);
    } else {
        clientsData[clientCount][0] = clientPID;
        clientsData[clientCount++][1] = clientQueueId;
        sprintf(msg->mtext, "%d", clientCount - 1);
    }

    if (msgsnd(clientQueueId, msg, MSG_SIZE, 0) == -1) {
        puts("Login request error");
        return;
    }
}

void execMirror(struct Msg *msg) {
    int clientQueueId = prepareMsg(msg);
    if (clientQueueId == -1) return;

    reverse(msg->mtext);
    strcat(msg->mtext, "\n");

    if (msgsnd(clientQueueId, msg, MSG_SIZE, 0) == -1) {
        puts("Mirror request error");
        return;
    }
}

void execCalc(struct Msg *msg, mtype rqType) {
    int clientQueueId;

    if ((clientQueueId = prepareMsg(msg)) == -1)
        return;

    char **rqArgv = arguments(msg->mtext);
    int result;

    switch (rqType) {
        case ADD:
            result = atoi(rqArgv[0]) + atoi(rqArgv[1]);
            sprintf(msg->mtext, "%d\n", result);
            break;
        case MUL:
            result = atoi(rqArgv[0]) * atoi(rqArgv[1]);
            sprintf(msg->mtext, "%d\n", result);
            break;
        case SUB:
            result = atoi(rqArgv[0]) - atoi(rqArgv[1]);
            sprintf(msg->mtext, "%d\n", result);
            break;
        case DIV:
            result = atoi(rqArgv[0]) / atoi(rqArgv[1]);
            sprintf(msg->mtext, "%d\n", result);
            break;
    }

    if (msgsnd(clientQueueId, msg, MSG_SIZE, 0) == -1) {
        puts("Calc request error");
        return;
    }
}

void execTime(struct Msg *msg) {
    int clientQueueId;
    if ((clientQueueId = prepareMsg(msg)) == -1)
        return;

    char buf[50];
    FILE *f = popen("date", "r");
    fgets(buf, sizeof(buf), f);
    pclose(f);
    strcpy(msg->mtext, buf);

    if (msgsnd(clientQueueId, msg, MSG_SIZE, 0) == -1) {
        puts("Time request error");
        return;
    }
}

void execEnd(struct Msg *msg) {
    active = 0;
}

void execQuit(struct Msg *msg) {
    int i;

    for (i = 0; i < clientCount; i++) {
        if (clientsData[i][0] == msg->senderPID)
            break;
    }

    if (i == clientCount) {
        printf("Client Not Found!\n");
        return;
    }

    for (i = i; i + 1 < clientCount; i++) {
        clientsData[i][0] = clientsData[i + 1][0];
        clientsData[i][1] = clientsData[i + 1][1];
    }

    clientCount--;
}

int prepareMsg(struct Msg *msg) {
    int clientQueueId;
    if ((clientQueueId = findQueueID(msg->senderPID)) == -1) {
        puts("Couldnt find client");
        return -1;
    }

    msg->senderPID = getpid();

    return clientQueueId;
}

int findQueueID(pid_t senderPID) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clientsData[i][0] == senderPID)
            return clientsData[i][1];
    }

    return -1;
}

void deleteQueue() {
    int tmp;
    if (publicID != -1) {
        tmp = msgctl(publicID, IPC_RMID, NULL);
        puts("Server queue deleted");
    }
}

char **arguments(char *line) {
    int argc = 0;
    char **argv = malloc(2 * sizeof(char *));

    char *current_arg = NULL;
    while (argc < 2 && (current_arg = strtok(current_arg == NULL ? line : NULL, " \n")) != NULL) {
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

void intHandler(int signum) {
    exit(0);
}
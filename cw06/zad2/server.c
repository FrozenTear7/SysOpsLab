#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <mqueue.h>
#include <ctype.h>
#include <fcntl.h>
#include <signal.h>

#include "info.h"

int active = 1;
int clientsData[MAX_CLIENTS][2];
int clientCount = 0;
mqd_t publicID = -1;

void execRequest(struct Msg *msg);

void execLogin(struct Msg *msg);

void execMirror(struct Msg *msg);

void execCalc(struct Msg *msg, mtype rqType);

void execTime(struct Msg *msg);

void execEnd(struct Msg *msg);

void execQuit(struct Msg *msg);

int findMQD(pid_t senderPID);

int prepareMsg(struct Msg *msg);

char **arguments(char *line);

void reverse(char *str);

void deleteQueue();

void intHandler(int signum);

int main(int argc, char **argv) {
    key_t publicKey;
    char *path;
    struct mq_attr currentState;
    struct mq_attr posixAttr;
    posixAttr.mq_maxmsg = MAX_MQSIZE;
    posixAttr.mq_msgsize = MSG_SIZE;
    Msg buff;

    atexit(deleteQueue);
    signal(SIGINT, intHandler);

    if ((publicID = mq_open(serverPath, O_RDONLY | O_CREAT | O_EXCL, 0666, &posixAttr)) == -1) {
        puts("Couldnt create public queue");
        return 1;
    }

    puts("Server waiting for requests");

    while (1) {
        if (active == 0) {
            mq_getattr(publicID, &currentState);
            if (currentState.mq_curmsgs == 0)
                return 1;
        }

        if (mq_receive(publicID, (char *) &buff, MSG_SIZE, NULL) == -1) {
            puts("Receiving message by server failed");
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
    int clientPID = msg->senderPID, clientMQD;
    char clientPath[15];
    sprintf(clientPath, "/%d", clientPID);

    if ((clientMQD = mq_open(clientPath, O_WRONLY)) == -1) {
        puts("Couldnt read clientMQD");
        return;
    }

    msg->mtype = INIT;
    msg->senderPID = getpid();

    if (clientCount > MAX_CLIENTS - 1) {
        puts("Maximum amount of clients reached");
        sprintf(msg->mtext, "%d", -1);
        if (mq_send(clientMQD, (char *) msg, MSG_SIZE, 1) == -1) {
            puts("Login response error");
            return;
        }
        if (mq_close(clientMQD) == -1) {
            puts("Couldnt close queue");
            return;
        }
    } else {
        clientsData[clientCount][0] = clientPID;
        clientsData[clientCount++][1] = clientMQD;
        sprintf(msg->mtext, "%d", clientCount - 1);
        if (mq_send(clientMQD, (char *) msg, MSG_SIZE, 1) == -1) {
            puts("Login response error");
            return;
        }
    }
}

void execMirror(struct Msg *msg) {
    int clientQueueId;
    if ((clientQueueId = prepareMsg(msg)) == -1)
        return;

    reverse(msg->mtext);
    strcat(msg->mtext, "\n");

    if (mq_send(clientQueueId, (char *) msg, MSG_SIZE, 1) == -1) {
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

    if (mq_send(clientQueueId, (char *) msg, MSG_SIZE, 1) == -1) {
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

    if (mq_send(clientQueueId, (char *) msg, MSG_SIZE, 1) == -1) {
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

    if (mq_close(clientsData[i][1]) == -1) {
        puts("Couldnt close client queue");
        return;
    }

    for (i = i; i + 1 < clientCount; i++) {
        clientsData[i][0] = clientsData[i + 1][0];
        clientsData[i][1] = clientsData[i + 1][1];
    }

    clientCount--;
}

int prepareMsg(struct Msg *msg) {
    int clientMQD = findMQD(msg->senderPID);
    if (clientMQD == -1) {
        printf("Client Not Found!\n");
        return -1;
    }

    msg->senderPID = getpid();

    return clientMQD;
}

int findMQD(pid_t senderPID) {
    for (int i = 0; i < clientCount; i++) {
        if (clientsData[i][0] == senderPID)
            return clientsData[i][1];
    }
    return -1;
}

void deleteQueue() {
    for (int i = 0; i < clientCount; i++) {
        if (mq_close(clientsData[i][1]) == -1) {
            printf("Couldnt close client %d queue\n", i);
        }

        kill(clientsData[i][0], SIGINT);
    }
    if (publicID > -1) {
        if (mq_close(publicID) == -1) {
            puts("Error closing public queue");
        } else
            puts("Server queue closed");

        if (mq_unlink(serverPath) == -1)
            puts("Error deleting public Queue");
        else
            puts("Server queue deleted");
    } else
        puts("No queues to delete");
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
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>

#define BUF_SIZE 1024

struct msgbuf {
    long mtype;
    char mtext[BUF_SIZE];
};

int main(void) {
    int msqid = msgget(IPC_PRIVATE, IPC_CREAT | 0600);

    struct msgbuf message;
    struct msgbuf message2;
    message.mtype = 5;
    message2.mtype = 6;
    memset(&(message.mtext), 0, BUF_SIZE * sizeof(char));
    memset(&(message2.mtext), 0, BUF_SIZE * sizeof(char));
    strcpy(message.mtext, "Witaj swiecie");
    strcpy(message2.mtext, "Cala naprzod");

    msgsnd(msqid, &message, sizeof(long) + (strlen(message.mtext) * sizeof(char)), 0);
    msgsnd(msqid, &message2, sizeof(long) + (strlen(message2.mtext) * sizeof(char)), 0);

    struct msgbuf newMessage;
    struct msgbuf newMessage2;
    if (msgrcv(msqid, &newMessage, BUF_SIZE, 5, 0) == -1) {
        return 1;
    }
    if (msgrcv(msqid, &newMessage2, BUF_SIZE, 6, 0) == -1) {
        return 1;
    }

    printf("%s\n", newMessage.mtext);
    printf("%s\n", newMessage2.mtext);

    msgctl(msqid, IPC_RMID, NULL);

    return 0;
}
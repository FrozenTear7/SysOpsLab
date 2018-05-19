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

    pid_t child1 = fork();
    if (child1 == 0) {
        printf("My pid: %d\n", (int)getpid());
        msgsnd(msqid, &message, sizeof(long) + (strlen(message.mtext) * sizeof(char)), 0);
        exit(0);
    } else {
        struct msgbuf newMessage;
        msgrcv(msqid, &newMessage, BUF_SIZE, 5, 0);
        printf("%s\n", newMessage.mtext);
    }

    pid_t child2 = fork();
    if (child2 == 0) {
        printf("My pid: %d\n", (int)getpid());
        msgsnd(msqid, &message2, sizeof(long) + (strlen(message2.mtext) * sizeof(char)), 0);
        exit(0);
    } else {
        struct msgbuf newMessage2;
        msgrcv(msqid, &newMessage2, BUF_SIZE, 6, 0);
        printf("%s\n", newMessage2.mtext);
    }

    wait(NULL);

    struct msqid_ds queueInfo;

    msgctl(msqid, IPC_STAT, &queueInfo);

    printf("\nIlosc komunikatow: %ld\n", queueInfo.msg_qnum);
    printf("Ilosc bajtow: %ld\n", queueInfo.msg_cbytes);
    printf("Ostatni pid: %d\n", queueInfo.msg_lspid);

    msgctl(msqid, IPC_RMID, NULL);

    return 0;
}
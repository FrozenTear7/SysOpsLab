#ifndef INFO_H
#define INFO_H

#define MAX_CLIENTS  10
#define PROJECT_ID 1
#define MAX_MTEXT_SIZE 50
#define MSG_SIZE sizeof(Msg) - sizeof(long)

typedef enum mtype {
    LOGIN = 1, MIRROR = 2, ADD = 3, MUL = 4, SUB = 5, DIV = 6, TIME = 7, END = 8, INIT = 9, QUIT = 10
} mtype;

typedef struct Msg {
    long mtype;
    pid_t senderPID;
    char mtext[MAX_MTEXT_SIZE];
} Msg;

#endif
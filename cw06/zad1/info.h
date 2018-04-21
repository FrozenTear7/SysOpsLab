#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#define MAX_CLIENTS  10
#define PROJECT_ID 1
#define MAX_CONT_SIZE 50

typedef enum mtype {
    LOGIN = 1, MIRROR = 2, ADD = 3, MUL = 4, SUB = 5, DIV = 6, TIME = 7, END = 8, INIT = 9
} mtype;

typedef struct Msg {
    long mtype;
    pid_t senderPID;
    char cont[MAX_CONT_SIZE];
} Msg;

const size_t MSG_SIZE = sizeof(Msg) - sizeof(long);

#endif
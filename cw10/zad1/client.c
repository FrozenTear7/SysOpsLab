#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>

char *clientName, *mode, *serverAddress;

void die(char *s) {
    perror(s);
    exit(1);
}

void sigintHandler() {
    exit(1);
}

void atexitHandler() {

}

int main(int argc, char **argv) {
    if (argc != 4)
        exit(1);

    signal(SIGINT, sigintHandler);
    atexit(atexitHandler);

    clientName = argv[1];
    mode = argv[2];
    serverAddress = argv[2];

    exit(0);
}
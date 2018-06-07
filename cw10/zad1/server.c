#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>

#define MAX_CLIENTS 14

char *clients[MAX_CLIENTS];

char *port, *unixPath;

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
    if (argc != 3)
        exit(1);

    signal(SIGINT, sigintHandler);
    atexit(atexitHandler);

    port = argv[1];
    unixPath = argv[2];

    exit(0);
}
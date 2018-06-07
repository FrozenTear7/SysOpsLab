#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>

#define PORT 8888

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
//    if (argc != 4)
//        exit(1);

    signal(SIGINT, sigintHandler);
    atexit(atexitHandler);

//    clientName = argv[1];
//    mode = argv[2];
//    serverAddress = argv[2];
    serverAddress = "127.0.0.1";

    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        exit(1);
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, serverAddress, &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        exit(1);
    }

    if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        exit(1);
    }


    while (1) {
        send(sock, hello, strlen(hello), 0);
        printf("Hello message sent\n");
        valread = read(sock, buffer, 1024);
        printf("%s\n", buffer);
        sleep(3);
    }

    exit(0);
}
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <signal.h>
#include <pthread.h>

#define MAX_CLIENTS 14
#define PORT 8888

pthread_t listener, pinger, reader;

int clients[MAX_CLIENTS];

char *port, *unixPath;
char buf[50];
int bufReady = 0;

void die(char *s) {
    perror(s);
    exit(1);
}

void sigintHandler() {
    exit(1);
}

void atexitHandler() {

}

void *listenerFunction() {
    int opt = 1;
    int master_socket, addrlen, new_socket, activity, valread, sd;
    int max_sd;
    struct sockaddr_in address;

    char buffer[1025];  //data buffer of 1K

    //set of socket descriptors
    fd_set readfds;

    //initialise all clients[] to 0 so not checked
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i] = 0;
    }

    //create a master socket
    if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(1);
    }

    //set master socket to allow multiple connections ,
    //this is just a good habit, it will work without this
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(1);
    }

    //type of socket created
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    //bind the socket to localhost port 8888
    if (bind(master_socket, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(1);
    }

    //try to specify maximum of 3 pending connections for the master socket
    if (listen(master_socket, 3) < 0) {
        perror("listen");
        exit(1);
    }

    //accept the incoming connection
    addrlen = sizeof(address);
    puts("Server ready");

    while (1) {
        //clear the socket set
        FD_ZERO(&readfds);

        //add master socket to set
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        //add child sockets to set
        for (int i = 0; i < MAX_CLIENTS; i++) {
            //socket descriptor
            sd = clients[i];

            //if valid socket descriptor then add to read list
            if (sd > 0)
                FD_SET(sd, &readfds);

            //highest file descriptor number, need it for the select function
            if (sd > max_sd)
                max_sd = sd;
        }

        //wait for an activity on one of the sockets , timeout is NULL ,
        //so wait indefinitely
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            printf("select error");
        }

        //If something happened on the master socket ,
        //then its an incoming connection
        if (FD_ISSET(master_socket, &readfds)) {
            if ((new_socket = accept(master_socket, (struct sockaddr *) &address, (socklen_t * ) & addrlen)) < 0) {
                perror("accept");
                exit(1);
            }

            //add new socket to array of sockets
            for (int i = 0; i < MAX_CLIENTS; i++) {
                //if position is empty
                if (clients[i] == 0) {
                    clients[i] = new_socket;
                    printf("Adding to list of sockets as %d\n", i);

                    break;
                }
            }
        }

        for (int i = 0; i < MAX_CLIENTS; i++) {
            sd = clients[i];

            if (FD_ISSET(sd, &readfds)) {
                if ((valread = read(sd, buffer, 1024)) == 0) {
                    printf("Closing sd: %d\n", sd);
                    close(sd);
                    clients[i] = 0;
                } else {
                    while (!bufReady) {
                        puts("waiting");
                        sleep(2);
                    }

                    puts("xd");

                    fflush(stdin);
                    printf("Sending to sd: %d\n", sd);

                    if (send(new_socket, buf, strlen(buf), 0) != strlen(buf)) {
                        perror("send");
                    }
                }
            }
        }
    }
}

void *pingerFunction() {

}

void *readerFunction() {
    while (fgets(buf, sizeof(buf), stdin) != NULL) {
        bufReady = 1;
    }
}

int main(int argc, char **argv) {
//    if (argc != 3)
//        exit(1);

    signal(SIGINT, sigintHandler);
    atexit(atexitHandler);

//    port = argv[1];
//    unixPath = argv[2];

    pthread_create(&listener, NULL, listenerFunction, NULL);
//    pthread_create(&pinger, NULL, pingerFunction, NULL);
    pthread_create(&reader, NULL, readerFunction, NULL);

    while (1) {}
}
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/un.h>

#define BUFLEN 512
#define PORT 8888

void die(char *s) {
    perror(s);
    exit(1);
}

int main(void) {
    struct sockaddr_in si_me, si_other;

    int s, i, slen = sizeof(si_other), recv_len;
    char buf[BUFLEN];
    char newBuf[BUFLEN];

    if ((s = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
        die("socket");
    }

    memset((char *) &si_me, 0, sizeof(si_me));

    si_me.sin_family = AF_UNIX;
    si_me.sin_port = htons(PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);

    printf("Waiting for data...\n");

    if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1) {
        die("recvfrom()");
    }

    memcpy(newBuf, buf + 7, strlen(buf) - 7);
    printf("%s\n", newBuf);

    close(s);
    return 0;
}
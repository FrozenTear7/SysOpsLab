#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char **argv) {
    FILE *fp = fopen(argv[1], "a");
    printf("My pid: %d\n", (int)getpid());

    for (int i = 0; i < atoi(argv[2]); i++) {
        char buffer[100], buffer2[50];
        sprintf(buffer, "Slave: %d - ", (int) getpid());
        FILE *f = popen("date", "r");
        fgets(buffer2, sizeof(buffer), f);
        pclose(f);
        strcat(buffer, buffer2);
        fprintf(fp, "%s", buffer);

        sleep(rand() % 3 + 2);
    }

    kill(getppid(), SIGRTMIN + 1);

    return 0;
}
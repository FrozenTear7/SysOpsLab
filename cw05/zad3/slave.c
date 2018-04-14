#define _GNU_SOURCE

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

int main(int argc, char **argv)
{
    FILE *fp = fopen(argv[1], "w");

    for (int i = 0; i < atoi(argv[2]); i++)
    {
        char buffer[100], buffer2[50];
        sprintf(buffer, "Slave: %d - ", (int)getpid());
        FILE *f = popen("date", "r");
        fgets(buffer2, sizeof(buffer), f);
        pclose(f);
        strcat(buffer, buffer2);
        //printf("HOLA HOLA SPEED BUMP\n");
        //printf("%s\n", buffer);

        fwrite(buffer, 1, sizeof(buffer), fp);

        sleep(rand() % 3 + 2);

        close(f);
    }

    kill(getppid(), SIGRTMIN + 1);

    return 0;
}
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
#include <time.h>
#include <math.h>

int main(int argc, char **argv)
{
    srand(time(NULL) * getpid());
    FILE *fd = open(argv[1], O_WRONLY);

    printf("Slave pid: %d", (int)getpid());

    for (int i = 0; i < atoi(argv[2]); i++)
    {
        printf("\n");
        char buffer[100];
        strcat(buffer, getpid());
        printf("%s\n", buffer);
        FILE *fp = popen("date", "r");
        char *ln = NULL;
        size_t len = 0;

        while (getline(&ln, &len, fp) != -1)
            strcat(buffer, ln);

        pclose(fp);

        write(fd, buffer, sizeof(buffer));

        sleep(1);
    }

    close(fd);

    return 0;
}
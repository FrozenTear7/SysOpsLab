#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

int main(int argc, char **argv)
{
    srand(time(NULL));
    FILE *fd = open(argv[1], "w");
    if (fd == NULL)
    {
        perror("Slave - Error opening fifo.txt");
        remove(fd);
        exit(1);
    }

    write(fd, "dupa", sizeof("dupa"));

    for (int i = 0; i < atoi(argv[2]); i++)
    {
        char buffer[100], buffer2[50];
        sprintf(buffer, "Slave: %d - ", (int)getpid());
        FILE *f = popen("date", "r");
        fgets(buffer2, sizeof(buffer), f);
        pclose(f);
        strcat(buffer, buffer2);
        //printf("HOLA HOLA SPEED BUMP\n");
        printf("%s\n", buffer);

        write(fd, buffer, sizeof(buffer));

        sleep(rand() % 3 + 2);

        close(f);
    }

    kill(getppid(), SIGRTMIN + 1);

    close(fd);

    return 0;
}
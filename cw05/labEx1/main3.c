//Paweł Mendroch

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
    int fd[2], fd2[2];
    pipe(fd);
    pipe(fd2);
    pid_t pid = fork();
    if (pid == 0)
    {
        close(fd[1]);
        close(fd2[0]);
        char buff[100];
        read(fd[0], buff, sizeof(buff));
        printf("%s\n", buff);
        write(fd2[1], strcat(buff, "!!!"), sizeof(buff));
    }
    else
    {
        close(fd[0]);
        close(fd2[1]);
        char buff[100];
        write(fd[1], "hello world", sizeof("hello world\n"));
        read(fd2[0], buff, sizeof(buff));
        printf("%s\n", buff);
    }

    return 0;
}

//Paweł Mendroch

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
    int fd[2];
    pipe(fd);
    pid_t pid = fork();
    if (pid != 0)
    {
        close(fd[1]);
        char buff[100];
        read(fd[0], buff, sizeof(buff));
        printf("%s", buff);
    }
    else
    {
        close(fd[0]);
        write(fd[1], "hello world\n", sizeof("hello world\n"));
    }

    return 0;
}

//1) Jesli dziecko nie wykona write z wiadomoscia do rodzica nie wydarzy sie nic, bufor pozostanie pusty i nic sie nie zostanie wypisane

//2&3) Podobnie jak poprzednie nie dzieje sie nic brak bledow, brak wiadomosci
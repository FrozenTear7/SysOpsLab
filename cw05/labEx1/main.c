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
    if (pid == 0)
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


//1) Jesli rodzic nie wykona write z wiadomoscia do dziecka nie wydarzy sie nic, bufor pozostanie pusty i nic sie nie zostanie wypisane

//2&3) Jesli w dziecku deskryptor do zapisu zostanie zamkniety nie bedzie moglo przesylac wiadomosci do rodzica przez potok, jednak uzywajac
//potoku nienazwanego nie jest to problem, gdyz dziala on w jedna strone, a rodzic ma wysylac wiadomosc do dziecka
//Tak samo jesli rodzicowi zamkniemy deskrpytor do odczytu w tym przypadku nie stanie sie nic jako ze rodzic jedynie posyla do dziecka
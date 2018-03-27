#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void handle_sigusr1(int signum)
{
    printf("Ale mi sygnal poslales panie zloty!!!\n");
}

int main()
{
    signal(SIGUSR1, handle_sigusr1);
    raise(SIGUSR1);
}
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int slavesReady = 0;

void slavesHandler(int signo)
{
  printf("Slave rdy");
  if (signo == SIGRTMIN + 1)
    slavesReady++;
}

int main(int argc, char **argv)
{
  if (argc < 3)
    return 1;

  signal(SIGRTMIN + 1, slavesHandler);

  char *masterArgv[3];
  masterArgv[0] = "./master";
  masterArgv[1] = "fifo2";
  masterArgv[2] = NULL;

  pid_t master = fork();
  if (master == 0)
  {
    execvp(masterArgv[0], masterArgv);
    return 0;
  }

  char *slaveArgv[4];
  slaveArgv[0] = "./slave";
  slaveArgv[1] = "fifo2";
  slaveArgv[2] = argv[2];
  slaveArgv[3] = NULL;

  for (int i = 0; i < atoi(argv[1]); i++)
  {
    pid_t slave = fork();
    if (slave == 0)
    {
      execvp(slaveArgv[0], slaveArgv);
      return 0;
    }
  }

  while (slavesReady < atoi(argv[1]))
  {
  }

  kill(master, SIGRTMIN);

  while (1)
  {
    waitpid(master, NULL, 0);
  }

  remove("fifo2");

  return 0;
}
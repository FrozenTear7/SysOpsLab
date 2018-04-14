#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <complex.h>

int main(int argc, char **argv)
{
  if (argc < 3)
    return 1;

  char *masterArgv[2];
  masterArgv[0] = "./master";
  masterArgv[1] = "/fifo";

  pid_t master = fork();
  if (master == 0)
  {
    execvp(masterArgv[0], masterArgv);
    return 0;
  }

  char *slaveArgv[3];
  slaveArgv[0] = "./slave";
  slaveArgv[1] = "/fifo";
  slaveArgv[2] = argv[2];

  for (int i = 0; i < atoi(argv[1]); i++)
  {
    pid_t slave = fork();
    if (slave == 0)
    {
      execvp(slaveArgv[0], slaveArgv);
      return 0;
    }
  }

  while (1)
  {
    wait(NULL);
  }

  return 0;
}
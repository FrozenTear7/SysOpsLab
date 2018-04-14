#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>

int maxPrograms = 10;
int maxProgramArgv = 10;

void runprogArr(char *line)
{
    int progCount = 0;
    char **progArr = malloc(maxPrograms * sizeof(char *));

    char *curProgram = NULL;
    while ((curProgram = strtok(curProgram == NULL ? line : NULL, "|")) != NULL)
    {
        if (progCount == maxPrograms)
            exit(1);

        progArr[progCount++] = curProgram;
    }

    pid_t *pids = malloc(progCount * sizeof(pid_t));
    for (int i = 0; i < progCount; i++)
        pids[i] = -1;

    int evenPipe[2];
    int oddPipe[2];

    for (int i = 0; i < progCount; i++)
    {
        int argc = 0;
        char **argv = malloc(maxProgramArgv * sizeof(char *));

        char *current_arg = NULL;
        while ((current_arg = strtok(current_arg == NULL ? progArr[i] : NULL, " \n")) != NULL)
        {
            if (argc == maxProgramArgv)
                exit(1);

            argv[argc++] = current_arg;
        }

        if (i % 2 == 0)
        {
            pipe(evenPipe);
            if (i != 0)
            {
                close(evenPipe[0]);
                close(evenPipe[1]);
            }
        }
        else
        {
            pipe(oddPipe);
            if (i != 1)
            {
                close(oddPipe[0]);
                close(oddPipe[1]);
            }
        }

        pids[i] = fork();
        if (pids[i] == 0)
        {
            if (i % 2 == 0)
            {
                close(evenPipe[0]);
                dup2(evenPipe[1], 1);
                if (i != 0)
                {
                    close(oddPipe[1]);
                    dup2(oddPipe[0], 0);
                }
                else
                {
                    close(oddPipe[0]);
                    dup2(oddPipe[1], 1);
                    close(evenPipe[1]);
                    dup2(evenPipe[0], 0);
                }

                execvp(argv[0], argv);
            }
        }

        for (int i = 0; i < progCount; ++i)
            waitpid(pids[i], NULL, 0);

        //free(pids);
        //free(progArr);
    }
}

int main(int argc, char **argv)
{
    if (!argv[1])
        return 1;

    FILE *fp;
    char *line;
    size_t len = 0;
    int read;

    fp = fopen(argv[1], "r");
    if (fp == NULL)
        return 1;

    while ((read = getline(&line, &len, fp)) != -1)
        runprogArr(line);

    close(fp);

    return 0;
}
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

    pid_t *pids = calloc(progCount, sizeof(pid_t));
    for (int i = 0; i < progCount; i++)
        pids[i] = -1;

    int fds[4] = {-1, -1, -1, -1};

    for (int i = 0; i < progCount; i++)
    {
        int argc = 0;
        char **argv = malloc(maxProgramArgv * sizeof(char*));

        char *current_arg = NULL;
        while ((current_arg = strtok(current_arg == NULL ? progArr[i] : NULL, " \n")) != NULL)
        {
            if (argc == maxProgramArgv)
                exit(1);

            argv[argc++] = current_arg;
        }

        fds[1] = fds[3];
        fds[0] = fds[2];

        pipe(fds + 2);

        pids[i] = fork();
        if (pids[i] == 0)
        {
            if (i > 0)
            {
                if (fds[0] != -1)
                {
                    dup2(fds[0], STDIN_FILENO);
                }

                if (fds[1] != -1)
                {
                    close(fds[1]);
                }
            }

            if (i < progCount - 1)
            {
                if (fds[2] != -1)
                {
                    close(fds[2]);
                }

                if (fds[3] != -1)
                {
                    dup2(fds[3], STDOUT_FILENO);
                }
            }

            execvp(argv[0], argv);
        }

        if (fds[1] != -1)
            close(fds[1]);
        if (fds[0] != -1)
            close(fds[0]);
    }

    for (int i = 0; i < progCount; ++i)
        waitpid(pids[i], NULL, 0);

    free(pids);
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
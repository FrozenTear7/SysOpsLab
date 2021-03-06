#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <sys/times.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
    if (!argv[1])
        return 1;
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    int read;

    fp = fopen(argv[1], "r");
    if (fp == NULL)
        return 1;

    while ((read = getline(&line, &len, fp)) != -1)
    {
        char *pch, *tmp = malloc(strlen(line) + 1);
        strcpy(tmp, line);

        pch = strtok(tmp, " ");
        int i = 0, count = 0;
        while (pch != NULL)
        {
            count++;
            pch = strtok(NULL, " ");
        }

        free(tmp);

        char *execArgv[count];
        pch = strtok(line, " ");

        while (i < count && pch != NULL)
        {
            execArgv[i] = pch;
            i++;
            pch = strtok(NULL, " ");
        }

        pid_t child_pid = fork();
        int status;

        if (child_pid == 0)
        {
            printf("Current exec: %s\n", execArgv[0]);

            execvp(execArgv[0], execArgv);
            exit(1);
        }
        else if (child_pid > 0)
        {
            waitpid(child_pid, &status, 0);
        }

        if (status != 0)
        {
            printf("Exec: %s gave an error! Status: %d\n", execArgv[0], status);
            fclose(fp);
            return 1;
        }

        printf("\n\n");
    }

    fclose(fp);
    if (line)
        free(line);

    return 0;
}
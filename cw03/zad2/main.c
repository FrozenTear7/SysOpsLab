#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
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
        char *pch, *tmp = line;

        pch = strtok(tmp, " ");
        int i = 0, count = 0;
        while (pch != NULL)
        {
            count++;
            pch = strtok(NULL, " ");
        }

        char *execArgv[count + 1];
        pch = strtok(line, " ");

        while (i < count && pch != NULL)
        {
            execArgv[i] = pch;
            i++;
            pch = strtok(NULL, " ");
        }

        pid_t child_pid = fork();
        if (child_pid == -1)
        {
            printf("Could not create child process!");
            exit(3);
        }
        else if (child_pid == 0)
        {
            printf("Current exec: %s\n", execArgv[0]);
            execvp(execArgv[0], execArgv);
            printf("Exec: %s gave an errno: %d!\n", execArgv[0], errno);
            exit(1);
        }
        else
        {
            int status;
            waitpid(child_pid, &status, 0);
            if (WIFEXITED(status) && (WEXITSTATUS(status) != 0))
            {
                printf("Error in: %s\n", execArgv[0]);
                exit(1);
            }
        }

        printf("\n\n");
    }

    fclose(fp);
    if (line)
        free(line);

    return 0;
}
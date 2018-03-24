#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char **argv)
{ /*
    char *const arg[3] = {"ls", "-l", NULL};
    char *const arg2[1] = {"echo", "xd"};
    //execvp("ls", arg);
    execvp("echo", arg2);*/

    FILE *fp;
    char *line = NULL;
    int len = 0, read;

    fp = fopen(argv[1], "r");
    if (fp == NULL)
        return 1;

    while ((read = getline(&line, &len, fp)) != -1)
    {
        char *pch, *execArgv[5] = {NULL, NULL, NULL, NULL, NULL}, *execName;
        pch = strtok(line, " ");
        int i = 0;
        while (i < 6 && pch != NULL)
        {
            if (i == 0)
                execName = pch;
            else
                execArgv[i] = pch;
            i++;
            pch = strtok(NULL, " ");
        }

        pid_t child_pid;
        child_pid = fork();
        printf("%d", child_pid);
        if (child_pid == 0)
        {
            printf("xd");
            printf("%s", execName);
            printf("%s, %s, %s, %s, %s, %s", execName, execArgv[0], execArgv[1], execArgv[3], execArgv[4], execArgv[5]);
            execvp(execName, execArgv);
        }
    }

    fclose(fp);
    if (line)
        free(line);

    return 0;
}
//Paweł Mendroch

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char ln[1024], command[200];
    FILE *fp;

    sprintf(command, "ps aux | wc -l");
    fp = popen(command, "r");
    while(fgets(ln, sizeof ln, fp) != NULL)
    {
        printf("%s", ln);
    }

    pclose(fp);
}

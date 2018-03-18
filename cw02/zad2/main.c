#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

char compare;
int ms_time;

void printFileInfo(char *fileName, struct stat *fileInfo, char *absolutePath) {
    printf("Information for %s\n", fileName);
    printf("---------------------------\n");
    printf("File Size: \t\t%d bytes\n", (int) fileInfo->st_size);
    printf("Full path is: %s\n", absolutePath);
    printf("File Permissions: \t");
    printf((S_ISDIR(fileInfo->st_mode)) ? "d" : "-");
    printf((fileInfo->st_mode & S_IRUSR) ? "r" : "-");
    printf((fileInfo->st_mode & S_IWUSR) ? "w" : "-");
    printf((fileInfo->st_mode & S_IXUSR) ? "x" : "-");
    printf((fileInfo->st_mode & S_IRGRP) ? "r" : "-");
    printf((fileInfo->st_mode & S_IWGRP) ? "w" : "-");
    printf((fileInfo->st_mode & S_IXGRP) ? "x" : "-");
    printf((fileInfo->st_mode & S_IROTH) ? "r" : "-");
    printf((fileInfo->st_mode & S_IWOTH) ? "w" : "-");
    printf((fileInfo->st_mode & S_IXOTH) ? "x" : "-");
    printf("\n");
    printf("Last modified: \t\t%lld", (long long) fileInfo->st_mtime);
    printf("\n\n");
}

void readDirectory(char *dirName) {
    DIR *d;
    d = opendir(dirName);
    struct dirent *dir;
    struct stat *fileStat;

    if (!d)
        return;

    dir = readdir(d);

    while (dir != NULL) {
        fileStat = malloc(sizeof(struct stat));
        if (lstat(dir->d_name, fileStat) >= 0) {
            char absolutePath[500];
            realpath(dir->d_name, absolutePath);

            if((strcmp(dir->d_name, ".") == 0) || (strcmp(dir->d_name, "..") == 0)) {
                dir = readdir(d);
                continue;
            }
            if (dir->d_type == DT_DIR) {
                printf("xd");
                readDirectory(dir->d_name);
            } else if (compare == '<') {
                if (fileStat->st_mtime < ms_time &&
                    dir->d_type == DT_REG) {
                    printFileInfo(dir->d_name, fileStat, absolutePath);
                }
            } else if (compare == '>') {
                if (fileStat->st_mtime > ms_time &&
                    dir->d_type == DT_REG) {
                    printFileInfo(dir->d_name, fileStat, absolutePath);
                }
            } else if (compare == '=') {
                if (fileStat->st_mtime == ms_time &&
                    dir->d_type == DT_REG) {
                    printFileInfo(dir->d_name, fileStat, absolutePath);
                }
            }
        }

        free(fileStat);
        dir = readdir(d);
    }

    closedir(d);
}

int main(int argc, char **argv) {
    char *dirName = ".";
    compare = '>';
    ms_time = 1500799341;

    readDirectory(dirName);

    return 0;
}
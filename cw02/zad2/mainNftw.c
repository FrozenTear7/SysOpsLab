#include <stdio.h>
#include <ftw.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdint.h>
#include <unistd.h>

char compare;
int ms_time;

void printInfo(const struct stat *fileInfo, const char *path, int typeflag) {
    if (typeflag == FTW_F) {
        printf("File path: %s", path);
        printf("---------------------------\n");
        printf("File Size: \t\t%d bytes\n", (int) fileInfo->st_size);
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
}

int printFileInfo(const char *path, const struct stat *fileInfo, int typeflag, struct FTW *ftwbuf) {
    if (compare == '<') {
        if (fileInfo->st_mtime < ms_time) {
            printInfo(fileInfo, path, typeflag);
        }
    } else if (compare == '>') {
        if (fileInfo->st_mtime > ms_time) {
            printInfo(fileInfo, path, typeflag);
        }
    } else if (compare == '=') {
        if (fileInfo->st_mtime == ms_time) {
            printInfo(fileInfo, path, typeflag);
        }
    }

    return 0;
}

int main3(int argc, char **argv) {
    char *dirName = argv[1], fullName[500];
    compare = argv[2][0];
    ms_time = atoi(argv[3]);

    if (_fullpath(fullName, dirName, 500) == NULL)
        return 1;

    if (nftw(fullName, printFileInfo, 20, FTW_PHYS) == -1) {
        return 1;
    }

    return 0;
}
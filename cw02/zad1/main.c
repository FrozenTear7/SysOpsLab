#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/times.h>

clock_t startTime;
clock_t endTime;
struct tms startCpu;
struct tms endCpu;

void startClock() {
    startTime = times(&startCpu);
}

void endClock(double *result) {
    static long clk = 0;
    clk = sysconf(_SC_CLK_TCK);
    endTime = times(&endCpu);
    result[0] = (endTime - startTime) / (double)clk;
    result[1] = (endCpu.tms_utime - startCpu.tms_utime) / (double)clk;
    result[2] = (endCpu.tms_stime - startCpu.tms_stime) / (double)clk;
}

void printTime(double *timer) {
    printf("\nExecution: user time %2.2f, sys time %2.2f\n", timer[1], timer[2]);
}

int main(int argc, char **argv) {
    srand(time(NULL));
    double timer[3];

    startClock();
    if (strcmp((char *) argv[1], "generate") == 0) {
        FILE *f = fopen((char *) argv[2], "w");
        if (f == NULL) {
            printf("Error opening file!\n");
            return (1);
        }

        int noRecords = atoi(argv[3]), recordSize = atoi(argv[4]), i, j;

        for (i = 0; i < noRecords; i++) {
            for (j = 0; j < recordSize; j++) {
                fprintf(f, "%c", 'A' + (rand() % 25));
            }
        }

        fclose(f);
    } else if (strcmp((char *) argv[1], "sort") == 0) {
        if (strcmp((char *) argv[5], "sys") == 0) {
            int f = open((char *) argv[2], O_RDWR);

            if (f < 0)
                return 1;

            char *buf1, *buf2, *keyArray;
            buf1 = malloc(atoi(argv[4]) * sizeof(char));
            buf2 = malloc(atoi(argv[4]) * sizeof(char));
            int noRecords = atoi(argv[3]), recordSize = atoi(argv[4]), i, j, key;

            for (i = 1; i < noRecords && f != EOF; i++) {
                lseek(f, i * recordSize, SEEK_SET);
                read(f, buf1, recordSize);
                keyArray = buf1;
                key = keyArray[0];
                j = i - 1;
                lseek(f, j * recordSize, SEEK_SET);
                read(f, buf2, recordSize);

                while (j >= 0 && buf2[0] > key) {
                    lseek(f, j * recordSize, SEEK_SET);
                    read(f, buf2, recordSize);

                    lseek(f, (j + 1) * recordSize, SEEK_SET);
                    write(f, buf2, recordSize);
                    j = j - 1;

                    lseek(f, j * recordSize, SEEK_SET);
                    read(f, buf2, recordSize);
                }

                lseek(f, (j + 1) * recordSize, SEEK_SET);
                write(f, keyArray, recordSize);
            }

            close(f);
        } else if (strcmp((char *) argv[5], "lib") == 0) {
            FILE *f = fopen((char *) argv[2], "r+");
            if (f == NULL) {
                printf("Error opening file!\n");
                return (1);
            }

            char *buf1, *buf2, *keyArray;
            buf1 = malloc(atoi(argv[4]) * sizeof(char));
            buf2 = malloc(atoi(argv[4]) * sizeof(char));
            int noRecords = atoi(argv[3]), recordSize = atoi(argv[4]), i, j, key;

            for (i = 1; i < noRecords && !feof(f); i++) {
                fseek(f, i * recordSize, SEEK_SET);
                fread(buf1, recordSize, 1, f);
                keyArray = buf1;
                key = keyArray[0];
                j = i - 1;
                fseek(f, j * recordSize, SEEK_SET);
                fread(buf2, recordSize, 1, f);

                while (j >= 0 && buf2[0] > key) {
                    fseek(f, j * recordSize, SEEK_SET);
                    fread(buf2, recordSize, 1, f);

                    fseek(f, (j + 1) * recordSize, SEEK_SET);
                    fwrite(buf2, recordSize, 1, f);
                    j = j - 1;

                    fseek(f, j * recordSize, SEEK_SET);
                    fread(buf2, recordSize, 1, f);
                }

                fseek(f, (j + 1) * recordSize, SEEK_SET);
                fwrite(keyArray, recordSize, 1, f);
            }

            fclose(f);
        } else {
            printf("Zly argument dla trybu!");
        }
    } else if (strcmp((char *) argv[1], "copy") == 0) {
        if (strcmp((char *) argv[6], "sys") == 0) {
            int f = open((char *) argv[2], O_RDWR);
            int f2 = open((char *) argv[3], O_WRONLY);
            if (f < 0 || f2 < 0) {
                printf("Error opening file!\n");
                return (1);
            }

            int noRecords = atoi(argv[4]), recordSize = atoi(argv[5]), i = 0;

            char *buffer;
            buffer = malloc(recordSize * sizeof(char));

            while (i < noRecords && f != EOF) {
                read(f, buffer, recordSize);
                write(f2, buffer, recordSize);
                i++;
            }

            close(f);
            close(f2);
        } else if (strcmp((char *) argv[6], "lib") == 0) {
            FILE *f = fopen((char *) argv[2], "r");
            FILE *f2 = fopen((char *) argv[3], "w");
            if (f == NULL || f2 == NULL) {
                printf("Error opening file!\n");
                return (1);
            }

            int noRecords = atoi(argv[4]), recordSize = atoi(argv[5]), i = 0;

            char *buffer;
            buffer = malloc(recordSize * sizeof(char));

            while (i < noRecords && !feof(f)) {
                fread(buffer, recordSize, 1, f);
                fwrite(buffer, recordSize, 1, f2);
                i++;
            }

            fclose(f);
            fclose(f2);
        }
    } else {
        printf("Nieznana komenda!");
    }

    endClock(timer);
    printTime(timer);

    return 0;
}
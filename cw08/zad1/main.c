#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <sys/times.h>

int **originalImage, **resultImage;
float **filterArr;
int noThreads;
int n, m, c;
int maxValue = -1;
clock_t startTime;
clock_t endTime;
struct tms startCpu;
struct tms endCpu;

int max(int a, int b) {
    if (a >= b)
        return a;
    else
        return b;
}

int min(int a, int b) {
    if (a < b)
        return a;
    else
        return b;
}

void *filterImage(void *arg) {
    int i = *((int *) arg);
    for (; i < n; i += noThreads) {
        for (int j = 0; j < m; j++) {
            float resultSum = 0;

            for (int a = 0; a < c; a++) {
                for (int b = 0; b < c; b++) {
                    resultSum += originalImage[min(max(0, i - ceil(c / 2) + a), n - 1)][min(max(0, j - ceil(c / 2) + b),
                                                                                            m - 1)] * filterArr[a][b];
                }
            }

            if (round(resultSum) > maxValue)
                maxValue = round(resultSum);

            resultImage[i][j] = round(resultSum);
        }
    }

    free(arg);

    return NULL;
}

int *lineArgv(char *line, int x) {
    int *resultArr = malloc(x * sizeof(int));
    int argCounter = 0;

    char *current_arg = NULL;
    while (argCounter < x && (current_arg = strtok(current_arg == NULL ? line : NULL, " \n")) != NULL) {
        resultArr[argCounter++] = atoi(current_arg);
    }

    return resultArr;
}

float *lineFloatArgv(char *line, int x) {
    float *resultArr = malloc(x * sizeof(float));
    int argCounter = 0;

    char *current_arg = NULL;
    while (argCounter < x && (current_arg = strtok(current_arg == NULL ? line : NULL, " \n")) != NULL) {
        resultArr[argCounter++] = atof(current_arg);
    }

    return resultArr;
}

void startClock() {
    startTime = times(&startCpu);
}

void endClock(double *result) {
    long clk = sysconf(_SC_CLK_TCK);
    endTime = times(&endCpu);
    result[0] = (endTime - startTime) / (double) clk;
    result[1] = (endCpu.tms_utime - startCpu.tms_utime) / (double) clk;
    result[2] = (endCpu.tms_stime - startCpu.tms_stime) / (double) clk;
}

void printTime(double *timer) {
    printf("\nExecution: real time: %2.2f\n", timer[0]);
}

int main(int argc, char **argv) {
    if (argc != 5)
        exit(1);

    double timer[3];
    time_t t;
    srand((unsigned) time(&t));

    noThreads = atoi(argv[1]);

    pthread_t *threadArr = malloc(noThreads * sizeof(pthread_t));

    //open image file

    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int lineCounter = 0;
    int i = 0;

    fp = fopen(argv[2], "r");
    if (fp == NULL)
        exit(1);

    while ((read = getline(&line, &len, fp)) != -1) {
        if (lineCounter == 1) {
            int *tmpResult = lineArgv(line, 2);
            m = tmpResult[0];
            n = tmpResult[1];

            originalImage = (int **) malloc(n * sizeof(int *));
            resultImage = (int **) malloc(n * sizeof(int *));

            for (int j = 0; j < n; j++) {
                originalImage[j] = (int *) malloc(m * sizeof(int));
                resultImage[j] = (int *) malloc(m * sizeof(int));
            }
        } else if (lineCounter > 2 && lineCounter <= n + 2) {
            int *tmpResult = lineArgv(line, m);

            for (int j = 0; j < m; j++) {
                originalImage[i][j] = tmpResult[j];
            }

            i++;
        }

        lineCounter++;
    }

    fclose(fp);

    //open filter file

    len = 0;
    lineCounter = 0;
    i = 0;

    fp = fopen(argv[3], "r");
    if (fp == NULL)
        exit(1);

    while ((read = getline(&line, &len, fp)) != -1) {
        if (lineCounter == 0) {
            c = atoi(line);

            filterArr = (float **) malloc(c * sizeof(float *));

            for (int j = 0; j < c; j++) {
                filterArr[j] = (float *) malloc(c * sizeof(float));
            }
        } else if (lineCounter <= c) {
            float *tmpResult = lineFloatArgv(line, c);

            for (int j = 0; j < c; j++) {
                filterArr[i][j] = tmpResult[j];
            }

            i++;
        }

        lineCounter++;
    }

    fclose(fp);

    //start timer

    startClock();

    //create threads to process the image

    for (int i = 0; i < noThreads; i++) {
        int *threadArg = malloc(sizeof(*threadArg));
        *threadArg = i;
        pthread_create(&threadArr[i], NULL, filterImage, (void *) threadArg);
    }

    //wait for the threads to finish their job

    for (int i = 0; i < noThreads; i++) {
        pthread_join(threadArr[i], NULL);
    }

    //end timer

    endClock(timer);
    printTime(timer);

    //write the result image to file

    fp = fopen(argv[4], "w");
    if (fp == NULL)
        exit(1);

    fprintf(fp, "P2\n");
    fprintf(fp, "%d %d\n", m, n);
    fprintf(fp, "%d\n", maxValue);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            fprintf(fp, "%d ", resultImage[i][j]);
        }
        fprintf(fp, "\n");
    }

    //free all the allocated memory

    for (int i = 0; i < n; i++) {
        free(originalImage[i]);
        free(resultImage[i]);
    }

    for (int i = 0; i < c; i++) {
        free(filterArr[i]);
    }

    free(originalImage);
    free(resultImage);
    free(filterArr);
    free(threadArr);

    exit(0);
}
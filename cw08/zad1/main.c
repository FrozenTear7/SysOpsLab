#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

int **originalImage, **resultImage;
float **filterArr;
int noThreads;
int n, m, c;

void *filterImage(void *i) {
    for (; i < n; i += noThreads) {
        for (int j = 0; j < m; j++) {
            resultImage[(int) i][j] = originalImage[(int) i][j];
        }
    }

    return NULL;
}

int *lineArgv(char *line, int n) {
    int *resultArr = malloc(n * sizeof(int));
    int argCounter = 0;

    char *current_arg = NULL;
    while ((current_arg = strtok(current_arg == NULL ? line : NULL, " \n")) != NULL) {
        resultArr[argCounter++] = atoi(current_arg);
    }

    return resultArr;
}

float *lineFloatArgv(char *line, int n) {
    float *resultArr = malloc(n * sizeof(int));
    int argCounter = 0;

    char *current_arg = NULL;
    while ((current_arg = strtok(current_arg == NULL ? line : NULL, " \n")) != NULL) {
        resultArr[argCounter++] = atof(current_arg);
    }

    return resultArr;
}

int main(int argc, char **argv) {
    if (argc != 5)
        exit(1);

    noThreads = atoi(argv[1]);

    pthread_t *threadArr = malloc(noThreads * sizeof(pthread_t));

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
            n = tmpResult[0];
            m = tmpResult[1];

            originalImage = (int **) malloc(n * sizeof(int *));
            resultImage = (int **) malloc(n * sizeof(int *));

            for (int j = 0; j < n; j++) {
                originalImage[j] = (int *) malloc(m * sizeof(int));
                resultImage[j] = (int *) malloc(m * sizeof(int));
            }
        } else if (lineCounter > 2) {
            int *tmpResult = lineArgv(line, n);

            for (int j = 0; j < n; j++) {
                //printf(" - %d - \n", tmpResult[j]);
                originalImage[i][j] = tmpResult[j];
            }
        }

        lineCounter++;
    }

    fclose(fp);

    len = 0;
    lineCounter = 0;
    i = 0;

    fp = fopen(argv[3], "r");
    if (fp == NULL)
        exit(1);

    while ((read = getline(&line, &len, fp)) != -1) {
        if (lineCounter == 0) {
            int *tmpResult = lineArgv(line, 1);
            c = tmpResult[0];

            filterArr = (float **) malloc(c * sizeof(float *));

            for (int j = 0; j < c; j++) {
                filterArr[j] = (float *) malloc(c * sizeof(float));
            }
        } else {
            float *tmpResult = lineFloatArgv(line, n);

            for (int j = 0; j < c; j++) {
                filterArr[i][j] = tmpResult[j];
                //printf(" - %.6f - \n", filterArr[i][j]);
            }
        }

        lineCounter++;
    }

    fclose(fp);

    for (int i = 0; i < noThreads; i++) {
        pthread_create(&threadArr[i], NULL, filterImage, (void *) i);
    }

    for (int i = 0; i < noThreads; i++) {
        pthread_join(threadArr[i], NULL);
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            printf("%s - %s", resultImage[i][j], originalImage[i][j]);
        }
        printf("\n");
    }

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
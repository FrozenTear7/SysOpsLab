#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <math.h>

int **originalImage, **resultImage;
float **filterArr;
int noThreads;
int n, m, c;

int max(int a, int b) {
    if (a > b)
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
                    resultSum =
                            originalImage[max(0, i - ceil(c / 2) + a)][max(0, j - ceil(c / 2) + b)] * filterArr[a][b];
                }
            }

            resultImage[i][j] = ceil(resultSum);
        }
    }

    free(arg);

    return NULL;
}

int *lineArgv(char *line, int n) {
    int *resultArr = malloc(n * sizeof(int));
    int argCounter = 0;

    char *current_arg = NULL;
    while (argCounter < n && (current_arg = strtok(current_arg == NULL ? line : NULL, " \n")) != NULL) {
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
            }

            i++;
        }

        lineCounter++;
    }

    fclose(fp);

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

//    for (int i = 0; i < n; i++) {
//        for (int j = 0; j < m; j++) {
//            printf("%d ", resultImage[i][j]);
//        }
//        printf("\n");
//    }

    //write the result image to file

    fp = fopen(argv[4], "w");
    if (fp == NULL)
        exit(1);

    fprintf(fp, "P2\n");
    fprintf(fp, "%d %d\n", m, n);
    //TO BE CORRECTED
    fprintf(fp, "255\n");

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
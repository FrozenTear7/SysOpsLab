#include <stdio.h>
#include <time.h>
#include "libblocks.h"

int main(int argc, char **argv) {
    int index = 4, i, j, countActions = 0;
    unsigned int arrSize = atoi(argv[1]), blockSize = atoi(argv[2]);
    char allocType = argv[3][0];
    char **testArr;
    time_t t;
    srand((unsigned) time(&t));

    // parse arguments

    while(countActions < 3) {
        if(argv[index][0] == '1') {
            printf("\nStworzenie tablicy wielkosci %d, o wielkosci %d blokow.\n", arrSize, blockSize);
            testArr = createArray(arrSize);

            for(i = 0; i < arrSize; i++) {
                char *tmp;
                tmp = calloc(blockSize, sizeof(char));
                for (j = 0; j < blockSize; j++) {
                    tmp[j] = (char) ((rand() % 26) + 65);
                }

                createBlock(testArr, tmp, i, blockSize);
            }  

            index++;
            countActions++;
        } else if(argv[index][0] == '2') {
            printf("\nWypisanie bloku o indeksie.\n", atoi(argv[index+1]));
            
            for(i = 0; i < blockSize; i++) {
                printf("%c, ", testArr[atoi(argv[index+1])][i]);
            }

            index += 2;
            countActions++;
        } else if(argv[index][0] == '3') {
            printf("\nUsuniecie, a nastepnie dodanie %d blokow.\n", atoi(argv[index+1]));
            
            for(i = 0; i < atoi(argv[index+1]); i++) {
                deleteBlock(testArr, i);
            }

            for(i = 0; i < atoi(argv[index+1]); i++) {
                char *tmp;
                tmp = calloc(blockSize, sizeof(char));
                for (j = 0; j < blockSize; j++) {
                    tmp[j] = (char) ((rand() % 26) + 65);
                }

                createBlock(testArr, tmp, i, blockSize);
            }

            index += 2;
            countActions++;
        } else if(argv[index][0] == '4') {
            printf("\nNaprzemienne usuniecie, a nastepnie dodanie %d blokow.\n", atoi(argv[index+1]));
            
            for(i = 0; i < atoi(argv[index+1]); i++) {
                deleteBlock(testArr, i);

                char *tmp;
                tmp = calloc(blockSize, sizeof(char));
                for (j = 0; j < blockSize; j++) {
                    tmp[j] = (char) ((rand() % 26) + 65);
                }

                createBlock(testArr, tmp, i, blockSize);
            }

            index += 2;
            countActions++;
        } else if(argv[index][0] == '5') {
            printf("\nOdszukanie bloku, ktorego suma jest najblizsza podanej liczbie %d\n", atoi(argv[index+1]));

            char *tmpArr = blockSumClosestToNumber(testArr, atoi(argv[index+1]), arrSize);

            for(i = 0; i < blockSize; i++) {
                printf("%c, ", tmpArr[i]);
            }

            index += 2;
            countActions++;
        }
    }

    return 0;
}
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

#define NUM_SAMPLING 500000

int generateCounter();
void printHistogram(int *hist);

int main(){
    clock_t start = clock();

    srand(time(NULL));   // Initialization, should only be called once.
    int hist[25] = {0};
    for (int i = 0; i < NUM_SAMPLING; i++){   
        hist[12+generateCounter()]++;
    }

    printHistogram(hist);

    clock_t finish = clock();
    printf("\n_____Running time was : %6.3fs_____\n", (double)(finish - start)/CLOCKS_PER_SEC);

    return 0;
}

int generateCounter(){
    int count = 0;
        for (int i = 0; i < 11; i++){
            int rand_num = rand() % 100;
            if (rand_num >= 49)
                count++;
            else
                count--;
        }
    return count;
}

void printHistogram(int *hist) {
    int histCopy[25] = {0}; // A copy of hist, with values range of 0 to 100.

    int max = 0; // find the maximum value of hist, this is needed for changing the range of histogram.
    for (int i = 0; i < 25; i++)
        if (hist[i] > max)
            max = hist[i];

    printf("Hist: ");
    for (int i = 0; i < 25; i++){ // store the values of hist in histCopy with range of 0 - ~100
        printf("%d, ", hist[i]);
        histCopy[i] = (hist[i] / (double) max) * 100;
    }
    printf("\n");
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < histCopy[i]; j++) {
            printf("*");
        }
        printf("\n"); 
    }
}

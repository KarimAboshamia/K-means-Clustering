#include <stdio.h>
#include <string.h>
#include <omp.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#define THREADS 4

float distance(float x1, float y1, float x2, float y2) {
    return sqrt(((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1)));
}

int getminidx(float *arr, int n) {
    float mn = arr[0];
    int idx = 0;
    for (int i = 1; i < n; ++i) {
        if (arr[i] < mn) {
            mn = arr[i];
            idx = i;
        }
    }
    return idx;
}

typedef struct Point {
    float x, y;

} point;

int main(void) {

    int id, j, i, N = 0, iterations = 0;
    float x, y, xc, yc, epsilon = 0.01f;
    FILE *openFile;
    point *input = (point *) malloc(10000 * sizeof(point));
    point *generatedNumbers = (point *) malloc(THREADS * sizeof(point));
    point **clusters = (point **) malloc(THREADS * sizeof(*clusters));
    int cluscount[THREADS];
    float distancwithcenter[10000][THREADS];

    srand(time(0));



    //Input (Point 1)
    if ((openFile = fopen("input.txt", "r")) == NULL) {
        printf("Error! opening file\n");
        // Program exits if the file pointer returns NULL.
        exit(1);
    }
    while (fscanf(openFile, "%f %f", &x, &y) != EOF) {
        input[N].x = x;
        input[N].y = y;
        N++;
    }

    for (i = 0; i < THREADS; i++) {
        clusters[i] = (point *) malloc(N * sizeof(clusters[i]));
        cluscount[i] = 0;
    }

    //Generated centroids (Point 2)
    for (i = 0; i < THREADS; i++) {
        generatedNumbers[i].x = rand() % 100;
        generatedNumbers[i].y = rand() % 100;
    }

//Repeat (Point 6)
while(iterations < 25 * THREADS) {

    for(int i = 0; i < THREADS; i++)
    {
        cluscount[i] = 0;
    }

    //Calculate the distance between each point and cluster centroid (Point 3)
    #pragma omp parallel num_threads(THREADS) private(i, j, x, y, xc, yc) shared(input, generatedNumbers, distancwithcenter)
    {
        #pragma omp for schedule(static)
        for (i = 0; i < N; ++i) {
            x = input[i].x;
            y = input[i].y;
            for (j = 0; j < THREADS; j++) {
                xc = generatedNumbers[j].x, yc = generatedNumbers[j].y;
                distancwithcenter[i][j] = distance(x, y, xc, yc);
            }
        }
    }

    //filter (Point 4)
    for (i = 0; i < N; ++i) {
        id = getminidx(distancwithcenter[i], THREADS);
        clusters[id][cluscount[id]].x = input[i].x;
        clusters[id][cluscount[id]].y = input[i].y;
        cluscount[id] += 1;
    }

    //calculate new mean/centroid (Point 5)
    #pragma omp parallel num_threads(THREADS) private(i, j, x, y, xc, yc) shared(generatedNumbers, clusters, cluscount)
    {
        #pragma omp for schedule(static)
        for(i = 0; i < THREADS; ++i) {
            xc = 0;
            yc = 0;
            for(j = 0; j < cluscount[i]; ++j) {
                xc += clusters[i][j].x;
                yc += clusters[i][j].y;
            }
            if(cluscount[i] > 0)
            {
                generatedNumbers[i].x = xc / cluscount[i];
                generatedNumbers[i].y = yc / cluscount[i];
            }
        }
    }
    iterations++;
}

for (i = 0; i < THREADS; ++i) {
    printf("cluster %d\n", i + 1);
    for (j = 0; j < cluscount[i]; ++j) {
        printf("%f %f\n", clusters[i][j].x, clusters[i][j].y);
    }
    printf("\n\n");
}




//Debug
    /*for (i = 0; i < THREADS; ++i) {
        printf("cluster %d\n", i + 1);
        for (j = 0; j < cluscount[i]; ++j) {
            printf("%f %f\n", clusters[i][j].x, clusters[i][j].y);
        }
        printf("\n\n");
    }


    for (i = 0; i <N ; ++i) {
        printf("%f %f\n",input[i].x,input[i].y);
    }
    printf("Centroied\n");


        //for(i=0;i<4;i++){
    //       printf("%f %f\n",generatedNumbers[i].x,generatedNumbers[i].y);
    //   }




    //printf("%f %f\n",generatedNumbers[0].x,generatedNumbers[0].y);
    //printf("%f %f\n",generatedNumbers[1].x,generatedNumbers[1].y);
    //printf("%f %f\n",generatedNumbers[2].x,generatedNumbers[2].y);
    //printf("%f %f\n",generatedNumbers[3].x,generatedNumbers[3].y);



    printf("\n\ndistances\n");
    for (i = 0; i < N; ++i) {
        for (j = 0; j < THREADS; ++j) {
            printf("%f ", distancwithcenter[i][j]);
        }
        printf("\n");
    }
*/

    free(input);
    free(generatedNumbers);
    fclose(openFile);
    for (int i = 0; i < THREADS; i++) {
        free(clusters[i]);
    }
    free(clusters);
}
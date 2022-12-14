/*
 * IT388
 * Floyd-Warshall Algorithm project
 * Curtis Bryant, Jake Soto, Dylan Sweeney, Nick Voss
 */


#include <stdio.h>
#include <omp.h>
#include <stdlib.h>

#define n 10 // The number of nodes
int distance[n][n]; // distance[i][j] is the length of the edge between i and j if it exists, or 0 if it does not
double start, elapsed;

/* printDistance()
*/

void printDistance();

void floyd_warshall();

int main(int argc, char *argv[]) {
    double start, stop;
    //initialize distance[][]
    int i, j;
    for (i = 0; i < n; ++i)
        for (j = 0; j < n; ++j)
            if (i == j)
                distance[i][j] = 0;
            else
                distance[i][j] = (int) (11.0 * rand() / (RAND_MAX + 1.0)); //number creator 0-10

    printf("Original Matrix:\n");
    printDistance(); // printout the new array

    start = omp_get_wtime();
    floyd_warshall();
    elapsed = omp_get_wtime() - start;

    printf("Updated Matrix:\n");
    printDistance(); // printout again the new array with shortest paths (uses floyd_warshall())

    printf("Elapsed Time:  %f seconds\n", elapsed);

    return 0;
}

void printDistance() {
    int i, j;
    if(n < 30){
        printf("    ");
        for (i = 0; i < n; ++i)
            printf("%4c", 'A' + i);
        printf("\n");
        for (i = 0; i < n; ++i) {
            printf("%4c", 'A' + i);
            for (j = 0; j < n; ++j)
                printf("%4d", distance[i][j]);
            printf("\n");
        }
        printf("\n");
    }
    else{
        printf("Graph too big\n");
    }
}

/*
	floyd_warshall() method
    -----
	after calling this function distance[i][j] will find the the minimum distance
	between i and j if it exists (i.e. if there's a path between i and j)
	or 0 if it does not exist
*/
void floyd_warshall() {
    int i, j, k;

    for (k = 0; k < n; ++k)
#pragma omp parallel for private(i, j)
            for (i = 0; i < n; ++i)
                for (j = 0; j < n; ++j)
                    /* If i and j are different nodes and if
                        the paths between i and k and between
                        k and j exist, do */
                    if ((distance[i][k] * distance[k][j] != 0) && (i != j))
                        /* See if you can't get a shorter path
                            between i and j by interspacing
                            k somewhere along the current
                            path */
                        if ((distance[i][k] + distance[k][j] < distance[i][j]) || (distance[i][j] == 0))
                            distance[i][j] = distance[i][k] + distance[k][j];
}
/*
 * IT388
 * Floyd-Warshall Algorithm project
 * Curtis Bryant, Jake Soto, Dylan Sweeney, Nick Voss
 *
 * MPI
 *
 */


#include <stdio.h>
#include <stdlib.h>

#define n 13 // The number of nodes
int distance[n][n]; // distance[i][j] is the length of the edge between i and j if it exists, or 0 if it does not
double start, elapsed;

void printDistance();

void floyd_warshall();

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Status status;
    int nproc, rank;
    MPI_Comm_size(comm, &nproc);
    MPI_Comm_rank(comm, &rank);

    double start, stop;
    //initialize distance[][]
    int i, j;
    for (i = 0; i < n; ++i)
        for (j = 0; j < n; ++j)
            if (i == j)
                distance[i][j] = 0;
            else
                distance[i][j] = (int) (11.0 * rand() / (RAND_MAX + 1.0)); //number creator 0-10

    printDistance(); // print the new array

    start = MPI_Wtime();
    floyd_warshall();
    elapsed = MPI_Wtime() - start;

    printDistance(); // print again the new array with the shortest paths

    printf("Elapsed Time:  %f seconds\n", elapsed);
    return 0;
}

void printDistance() {
    int i, j;
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
        for (i = 0; i < n; ++i)
            if (i % nproc == rank) {
                for (j = 0; j < n; ++j)
                    /* If i and j are different nodes and if
                        the paths between i and k and between
                        k and j exist, do */
                    if ((distance[i][k] * distance[k][j] != 0) && (i != j))
                        /* See if you can't get a shorter path
                            between i and j by interspacing
                            k somewhere along the current path */
                        if ((distance[i][k] + distance[k][j] < distance[i][j]) || (distance[i][j] == 0))
                            distance[i][j] = distance[i][k] + distance[k][j];
            }
}
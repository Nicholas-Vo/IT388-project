#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <mpi.h>

#define V 4
#define INF INT_MAX

void printSolution(int final[][V]);
void floydWarshall(int graph[][V], int rank, int size);


int main(int argc, char* argv[])
{
    int dist[V][V], final[V][V];
    int size, rank;
    double start, elapsed;

    MPI_Init(&argc, &argv);
    MPI_Comm comm; MPI_Status status; comm = MPI_COMM_WORLD;
    MPI_Comm_size(comm, &size);
    MPI_Comm_rank(comm, &rank);

    // Define the input graph
    int graph[V][V] = { {0,   5,  INF, 10},
                        {INF, 0,   3, INF},
                        {INF, INF, 0,   1},
                        {INF, INF, INF, 0}
                      };

    //Start time measurement
    start = MPI_Wtime();

    // Run the Floyd-Warshall algorithm on the input graph
    floydWarshall(graph, rank, size);

    //Calculate time measurements
    elapsed = MPI_Wtime() - start;

    //Print the final solution
    printSolution(final);

    //Prints out elapsed time
    printf("Elapsed time: %f seconds, Number of processors: %d\n", elapsed, size);

    MPI_Finalize();

    return 0;
}


void floydWarshall(int graph[][V], int rank, int size)
{
    int i, j, k;
    int start = rank * V / size;
    int end = (rank + 1) * V / size;

    // Initialize the distance matrix with the values from the input graph
    for (i = start; i < end; i++)
        for (j = 0; j < V; j++)
            dist[i][j] = graph[i][j];

    for (k = 0; k < V; k++)
    {
        // Broadcast the current value of k to all processes
        MPI_Bcast(&k, 1, MPI_INT, 0, MPI_COMM_WORLD);

        for (i = start; i < end; i++)
        {
            for (j = 0; j < V; j++)
            {
                if (dist[i][k] != INF && dist[k][j] != INF &&
                    dist[i][k] + dist[k][j] < dist[i][j])
                    dist[i][j] = dist[i][k] + dist[k][j];
            }
        }

        // Gather the updated distances from each process and update the global distance matrix
        MPI_Allgather(&dist[start][0], end - start, MPI_INT, &final[0][0], end - start, MPI_INT, MPI_COMM_WORLD);
    }
}

void printSolution(int final[][V])
{
    int i, j;
    // Print if the matrix is less than 12
    if(V < 12){
        for (i = 0; i < V; i++){
            for (j = 0; j < V; j++){
                if (dist[i][j] == INF)
                    printf("%7s", "INF");
                else
                    printf("%7d", dist[i][j]);
            }
            printf("\n");
        }
    }
}
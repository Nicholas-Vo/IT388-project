#include <stdio.h>
#include "mpi.h"
#include <stdlib.h>

#define MASTER 0
#define WORKER_ID 1
#define DEST_ID 2
#define n 25 /* Then number of nodes */

int dist[n][n];

void print() {
    printf("    ");
    int i;
    for (i = 0; i < n; ++i)
        printf("%4c", 'A' + i);
    printf("\n");
    int j;
    for (i = 0; i < n; ++i) {
        printf("%4c", 'A' + i);
        for (j = 0; j < n; ++j)
            printf("%4d", dist[i][j]);
        printf("\n");
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    MPI_Status status;
    MPI_Init(&argc, &argv);

    int rank, nproc;
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int balance, slice;
    if (nproc == 1) { // prevent division by 0
        balance = n % 1;
        slice = (n - balance) / 1;
    } else {
        balance = n % (nproc - 1);
        slice = (n - balance) / (nproc - 1);
    }

    if (rank == MASTER) {
        int disable = 0;
        int t = 3;
        int result[t];
        int i, j;
        /* init */
        for (i = 0; i < n; ++i) {
            for (j = 0; j < n; ++j) {
                if (i == j) {
                    dist[i][j] = 0;
                } else {
                    dist[i][j] = (int) (11.0 * rand() / (RAND_MAX + 1.0));
                }
            }
        }

        if (n < 21) {
            print();
        }

        double start = MPI_Wtime();
        for (i = 1; i < nproc; i++)
            MPI_Send(&dist, n * n, MPI_INT, i, WORKER_ID, MPI_COMM_WORLD);

        do {
            MPI_Recv(&result, t, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            if (status.MPI_TAG == DEST_ID) {
                disable++;
            } else if (dist[result[1]][result[2]] > result[0]) {
                dist[result[1]][result[2]] = result[0];
            }
        } while (disable < nproc - 1);

        double end = MPI_Wtime();
        if (n < 21) {
            print();
        }

        printf("total time %f \n", end - start);
    } else {
        int i, j, k, t = 3;
        int out[t];
        MPI_Recv(&dist, n * n, MPI_INT, MASTER, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        if (rank + 1 != nproc) {
            balance = 0;
        }
        for (k = slice * (rank - 1); k < slice * (rank - 1) + slice + balance; ++k) {
            for (i = 0; i < n; ++i) {
                for (j = 0; j < n; ++j) {
                    if ((dist[i][k] * dist[k][j] != 0) && (i != j))
                        if ((dist[i][k] + dist[k][j] < dist[i][j]) || (dist[i][j] == 0)) {
                            dist[i][j] = dist[i][k] + dist[k][j];
                            out[0] = dist[i][j];
                            out[1] = i;
                            out[2] = j;
                            MPI_Send(&out, t, MPI_INT, MASTER, 0, MPI_COMM_WORLD);
                        }
                }
            }
        }
        MPI_Send(0, 0, MPI_INT, MASTER, DEST_ID, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
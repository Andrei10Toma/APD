#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define MASTER 0

int main(int argc, char *argv[])  {
    int procs, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int number = 2 * procs - 2 * rank;

    for (int i = 0; i < procs; i++) {
        if (i % 2 == 0) {
            if (rank % 2 == 0) {
                if (rank != procs - 1) {
                    int recv_number;
                    MPI_Send(&number, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
                    MPI_Recv(&recv_number, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, NULL);
                    number = MIN(number, recv_number);
                }
            } else {
                if (rank != 0) {
                    int recv_number;
                    MPI_Send(&number, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);
                    MPI_Recv(&recv_number, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, NULL);
                    number = MAX(number, recv_number);
                }
            }
        } else {
            if (rank % 2 != 0) {
                if (rank != procs - 1) {
                    int recv_number;
                    MPI_Send(&number, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
                    MPI_Recv(&recv_number, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, NULL);
                    number = MIN(number, recv_number);
                }
            } else {
                if (rank != 0) {
                    int recv_number;
                    MPI_Send(&number, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);
                    MPI_Recv(&recv_number, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, NULL);
                    number = MAX(number, recv_number);
                }
            }
        }
    }

    printf("%d - %d\n", rank, number);
    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == MASTER) {
        printf("%d ", number);
        for (int i = 0; i < procs - 1; i++) {
            int recv_number;
            MPI_Recv(&recv_number, 1, MPI_INT, 1, i + 1, MPI_COMM_WORLD, NULL);
            printf("%d ", recv_number);
        }
        printf("\n");
    } else if (rank == procs - 1) {
        MPI_Send(&number, 1, MPI_INT, rank - 1, rank, MPI_COMM_WORLD);
    } else {
        for (int i = 0; i < procs - rank - 1; i++) {
            int recv_number;
            MPI_Recv(&recv_number, 1, MPI_INT, rank + 1, procs - i - 1, MPI_COMM_WORLD, NULL);
            MPI_Send(&recv_number, 1, MPI_INT, rank - 1, procs - i - 1, MPI_COMM_WORLD);
        }
        MPI_Send(&number, 1, MPI_INT, rank - 1, rank, MPI_COMM_WORLD);
    }


    MPI_Finalize();
}
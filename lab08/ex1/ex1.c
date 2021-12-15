#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main (int argc, char *argv[])
{
    int  numtasks, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    int recv_num;
    MPI_Status status;
    srand(time(NULL));

    // First process starts the circle.
    if (rank == 0) {
        int number = rand();
        // First process starts the circle.
        // Generate a random number.
        MPI_Send(&number, 1, MPI_INT, 1, 1, MPI_COMM_WORLD);
        printf("Rank 0 sent the %d\n", number);
        // Send the number to the next process.
        MPI_Recv(&recv_num, 1, MPI_INT, numtasks - 1, 1, MPI_COMM_WORLD, &status);
        printf("Rank 0 received the %d\n", recv_num);

    } else if (rank == numtasks - 1) {
        // Last process close the circle.
        // Receives the number from the previous process.
        MPI_Recv(&recv_num, 1, MPI_INT, numtasks - 2, 1, MPI_COMM_WORLD, &status);
        printf("Rank %d received %d\n", rank, recv_num);
        // Increments the number.
        recv_num++;
        // Sends the number to the first process.
        MPI_Send(&recv_num, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
        printf("Rank %d sent the %d\n", rank, recv_num);
    } else {
        // Middle process.
        MPI_Recv(&recv_num, 1, MPI_INT, rank - 1, 1, MPI_COMM_WORLD, &status);
        printf("Rank %d received %d\n", rank, recv_num);
        // Receives the number from the previous process.
        // Increments the number.
        recv_num++;
        // Sends the number to the next process.
        MPI_Send(&recv_num, 1, MPI_INT, rank + 1, 1, MPI_COMM_WORLD);
        printf("Rank %d sent the %d\n", rank, recv_num);
    }

    MPI_Finalize();

}


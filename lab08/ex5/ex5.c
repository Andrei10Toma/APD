#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROOT 0

int main (int argc, char *argv[])
{
    int  numtasks, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    srand(time(NULL));

    // Checks the number of processes allowed.
    if (numtasks != 2) {
        printf("Wrong number of processes. Only 2 allowed!\n");
        MPI_Finalize();
        return 0;
    }

    // How many numbers will be sent.
    int send_numbers = 10;

    if (rank == ROOT) {
        for (int i = 0; i < send_numbers; i++) {
            // Generate the random numbers.
            int random_value = rand() % 20;
            // Generate the random tags.
            int random_tag = rand() % 13;
            // Sends the numbers with the tags to the second process.
            MPI_Send(&random_value, 1, MPI_INT, 1, random_tag, MPI_COMM_WORLD);
            printf("ROOT sent the value %d with the tag %d\n", random_value, random_tag);
        }
    } else {

        // Receives the information from the first process.
        
        for (int  i = 0; i < send_numbers; i++) {
            MPI_Status status;
            int recv_value;
            MPI_Recv(&recv_value, 1, MPI_INT, ROOT, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            // Prints the numbers with their corresponding tags.
            printf("Received the value %d with the tag %d\n", recv_value, status.MPI_TAG);
        }
    }

    MPI_Finalize();

}


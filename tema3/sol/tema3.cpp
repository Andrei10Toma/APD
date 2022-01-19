#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define NUMBER_OF_CLUSTERS  3
#define MASTER              0
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

int is_coordinator(int rank) {
    return rank == 0 || rank == 1 || rank == 2;
}

void log_sent_message(int source, int destination) {
    printf("M(%d,%d)\n", source, destination);
}

FILE* open_cluster_file(int rank) {
    switch (rank) {
    case 0:
        return fopen("cluster0.txt", "r");
    
    case 1:
        return fopen("cluster1.txt", "r");

    case 2:
        return fopen("cluster2.txt", "r");

    default:
        return NULL;
    }
}

int *read_cluster(FILE *cluster_file, int *n) {
    if (cluster_file == NULL)
        return NULL;

    fscanf(cluster_file, "%d", n);
    int *cluster = (int *)calloc(*n, sizeof(int));

    for (int i = 0; i < *n; i++)
        fscanf(cluster_file, "%d", &cluster[i]);

    return cluster;
}

void inform_workers_about_the_coordinator(int *cluster, int n, int rank) {
    for (int i = 0; i < n; i++) {
        MPI_Send(&rank, 1, MPI_INT, cluster[i], 0, MPI_COMM_WORLD);
        log_sent_message(rank, cluster[i]);
    }
}

void inform_the_other_coordinators_about_topology(int *cluster, int n, int rank) {
    for (int i = 0; i < NUMBER_OF_CLUSTERS; i++) {
        if (i != rank) {
            // send the length of the vector first
            MPI_Send(&n, 1, MPI_INT, i, 3, MPI_COMM_WORLD);
            log_sent_message(rank, i);
            MPI_Send(cluster, n, MPI_INT, i, 0, MPI_COMM_WORLD);
            log_sent_message(rank, i);
        }
    }
}

int print_topology_if_received(int **topology, int *topology_sizes, int rank) {
    int topology_received = 1;
    for (int i = 0; i < NUMBER_OF_CLUSTERS; i++) {
        if (topology[i] == NULL)
            topology_received = 0;
    }

    if (topology_received) {
        printf("%d -> ", rank);
        for (int i = 0; i < NUMBER_OF_CLUSTERS; i++) {
            printf("%d:", i);
            for (int j = 0; j < topology_sizes[i]; j++) {
                printf(j == topology_sizes[i] - 1 ? "%d " : "%d,", topology[i][j]);
            }
        }
        printf("\n");
    }

    return topology_received;
}

void send_topology_to_workers(int **topology, int rank, int *topology_sizes) {
    int *cluster = topology[rank];
    // for every worker of the cluster send the topology to him
    for (int i = 0; i < topology_sizes[rank]; i++) {
        for (int j = 0; j < NUMBER_OF_CLUSTERS; j++) {
            // send the size of the topology first
            MPI_Send(&topology_sizes[j], 1, MPI_INT, cluster[i], 3 + j, MPI_COMM_WORLD);
            log_sent_message(rank, cluster[i]);
            // send the topology to the workers
            MPI_Send(topology[j], topology_sizes[j], MPI_INT, cluster[i], j, MPI_COMM_WORLD);
            log_sent_message(rank, cluster[i]);
        }
    }
}

void coordinators_receive_topology(int **topology, int rank, int *topology_sizes) {
    for (int i = 0; i < NUMBER_OF_CLUSTERS; i++) {
        if (i != rank) {
            // first receive the length of the vector
            int received_n;
            MPI_Recv(&received_n, 1, MPI_INT, i, 3, MPI_COMM_WORLD, NULL);
            topology[i] = (int *)calloc(received_n, sizeof(int));
            topology_sizes[i] = received_n;
            MPI_Recv(topology[i], received_n, MPI_INT, i, 0, MPI_COMM_WORLD, NULL);
            int topology_received = print_topology_if_received(topology, topology_sizes, rank);
            if (topology_received)
                send_topology_to_workers(topology, rank, topology_sizes);
        }
    }
}

void workers_receive_topology(int coordinator, int **topology, int *topology_sizes, int rank) {
    for (int i = 0; i < NUMBER_OF_CLUSTERS; i++) {
        int received_size;
        MPI_Recv(&received_size, 1, MPI_INT, coordinator, 3 + i, MPI_COMM_WORLD, NULL);
        topology[i] = (int *)calloc(received_size, sizeof(int));
        topology_sizes[i] = received_size;
        MPI_Recv(topology[i], received_size, MPI_INT, coordinator, i, MPI_COMM_WORLD, NULL);
        print_topology_if_received(topology, topology_sizes, rank);
    }
}

int *generate_vector(int n) {
    int *v = (int *)calloc(n, sizeof(int));
    for (int i = 0; i < n; i++) {
        v[i] = i;
    }

    return v;
}

int compute_number_of_workers(int *topology_sizes) {
    int number_of_workers = 0;
    for (int i = 0; i < NUMBER_OF_CLUSTERS; i++) {
        number_of_workers += topology_sizes[i];
    }

    return number_of_workers;
}

void send_information_to_the_coordinators(int n, int *v, int rank) {
    for (int i = 1; i < NUMBER_OF_CLUSTERS; i++) {
        MPI_Send(&n, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        log_sent_message(rank, i);
        MPI_Send(v, n, MPI_INT, i, 2, MPI_COMM_WORLD);
        log_sent_message(rank, i);
    }
}

void send_information_to_the_workers(int n, int *v, int rank, int *cluster, int cluster_size) {
    for (int i = 0; i < cluster_size; i++) {
        MPI_Send(&n, 1, MPI_INT, cluster[i], 1, MPI_COMM_WORLD);
        log_sent_message(rank, cluster[i]);
        MPI_Send(v, n, MPI_INT, cluster[i], 0, MPI_COMM_WORLD);
        log_sent_message(rank, cluster[i]);
    }
}

void double_vector_and_send_to_coordinator(int rank, int n, int number_of_workers, int *v, int coordinator) {
    int start = (rank - 3) * (double)n / number_of_workers;
    int end = MIN((rank - 2) * (double)n / number_of_workers, n);
    for (int i = start; i < end; i++) {
        v[i] *= 2;
    }
    // send the start and end so that the master will know the positions
    // where the vector should be changed
    MPI_Send(&start, 1, MPI_INT, coordinator, 0, MPI_COMM_WORLD);
    log_sent_message(rank, coordinator);
    MPI_Send(&end, 1, MPI_INT, coordinator, 1, MPI_COMM_WORLD);
    log_sent_message(rank, coordinator);
    MPI_Send(v, n, MPI_INT, coordinator, 2, MPI_COMM_WORLD);
    log_sent_message(rank, coordinator);
}

void master_receive_changed_vector_from_worker_cluster(int n, int cluster_size, int *v) {
    for (int i = 0; i < cluster_size; i++) {
        int start, end;
        int *v_recv = (int *)calloc(n, sizeof(int));
        MPI_Status status;
        MPI_Recv(&start, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&end, 1, MPI_INT, status.MPI_SOURCE, 1, MPI_COMM_WORLD, NULL);
        MPI_Recv(v_recv, n, MPI_INT, status.MPI_SOURCE, 2, MPI_COMM_WORLD, NULL);
        for (int i = start; i < end; i++)
            v[i] = v_recv[i];
    }
}

void receive_the_changed_vector_and_send_it_to_master(int cluster_size, int n, int rank) {
    for (int i = 0; i < cluster_size; i++) {
        int start, end;
        int *v_recv = (int *)calloc(n, sizeof(int));
        MPI_Status status;
        MPI_Recv(&start, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&end, 1, MPI_INT, status.MPI_SOURCE, 1, MPI_COMM_WORLD, NULL);
        MPI_Recv(v_recv, n, MPI_INT, status.MPI_SOURCE, 2, MPI_COMM_WORLD, NULL);
        MPI_Send(&start, 1, MPI_INT, MASTER, i + cluster_size, MPI_COMM_WORLD);
        log_sent_message(rank, MASTER);
        MPI_Send(&end, 1, MPI_INT, MASTER, i + 2 * cluster_size, MPI_COMM_WORLD);
        log_sent_message(rank, MASTER);
        MPI_Send(v_recv, n, MPI_INT, MASTER, i + 3 * cluster_size, MPI_COMM_WORLD);
        log_sent_message(rank, MASTER);
    }
}

void receive_the_changed_vector_from_the_other_coordinators(int *topology_sizes, int n, int *v) {
    for (int i = 1; i < NUMBER_OF_CLUSTERS; i++) {
        for (int j = 0; j < topology_sizes[i]; j++) {
            int start, end;
            int *v_recv = (int *)calloc(n, sizeof(int));
            MPI_Recv(&start, 1, MPI_INT, i, j + topology_sizes[i], MPI_COMM_WORLD, NULL);
            MPI_Recv(&end, 1, MPI_INT, i, j + 2 * topology_sizes[i], MPI_COMM_WORLD, NULL);
            MPI_Recv(v_recv, n, MPI_INT, i, j + 3 * topology_sizes[i], MPI_COMM_WORLD, NULL);
            for (int i = start; i < end; i++) {
                v[i] = v_recv[i];
            }
        }
    }
}

int main(int argc, char *argv[]) {
    int procs, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    FILE *cluster_file = open_cluster_file(rank);
    int **topology = (int **)calloc(NUMBER_OF_CLUSTERS, sizeof(int *));
    int *topology_sizes = (int *)calloc(NUMBER_OF_CLUSTERS, sizeof(int));
    int coordinator = -1; 
    if (is_coordinator(rank)) {
        int n;
        MPI_Status status;
        // the coordinator will have his workers in the cluster array
        topology[rank] = read_cluster(cluster_file, &n);
        // send the information to the workers in the cluster
        inform_workers_about_the_coordinator(topology[rank], n, rank);
        inform_the_other_coordinators_about_topology(topology[rank], n, rank);
        topology_sizes[rank] = n;
        // first the coordinators will receive the topology
        coordinators_receive_topology(topology, rank, topology_sizes);
    } else {
        // receive the information about the coordinator
        MPI_Recv(&coordinator, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, NULL);
        workers_receive_topology(coordinator, topology, topology_sizes, rank);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    int number_of_workers = compute_number_of_workers(topology_sizes);
    if (rank == MASTER) {
        int n = atoi(argv[1]);
        int *v = generate_vector(n);
        // send the generated information to the other coordinates
        send_information_to_the_coordinators(n, v, rank);
        // send the information to the workers to compute the information
        send_information_to_the_workers(n, v, rank, topology[rank], topology_sizes[rank]);
        master_receive_changed_vector_from_worker_cluster(n, topology_sizes[rank], v);
        receive_the_changed_vector_from_the_other_coordinators(topology_sizes, n, v);
        printf("Rezultat: ");
        for (int i = 0; i < n; i++) {
            printf("%d ", v[i]);
        }
        printf("\n");
    } else if (is_coordinator(rank)) {
        // the other coordinates receive the information
        int n;
        MPI_Recv(&n, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, NULL);
        int *v = (int *)calloc(n, sizeof(int));
        MPI_Recv(v, n, MPI_INT, MASTER, 2, MPI_COMM_WORLD, NULL);
        send_information_to_the_workers(n, v, rank, topology[rank], topology_sizes[rank]);
        receive_the_changed_vector_and_send_it_to_master(topology_sizes[rank], n, rank);
    } else {
        // the workers receive the information and will do the job
        int n;
        MPI_Recv(&n, 1, MPI_INT, coordinator, 1, MPI_COMM_WORLD, NULL);
        int *v = (int *)calloc(n, sizeof(int));
        MPI_Recv(v, n, MPI_INT, coordinator, 0, MPI_COMM_WORLD, NULL);
        // doing this the coeficient will start from 0
        double_vector_and_send_to_coordinator(rank, n, number_of_workers, v, coordinator);
    }
    MPI_Finalize();
}

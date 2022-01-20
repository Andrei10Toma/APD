#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MASTER 0
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

int is_vowel(char ch) {
    return ch == 'a' || ch == 'e' || ch == 'i' || ch == 'o' || ch == 'u' ||
		ch == 'A' || ch == 'E' || ch == 'I' || ch == 'O' || ch == 'U';
}

int main(int argc, char *argv[])  {
    int procs, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == MASTER) {
        FILE *file = fopen("file.in", "r");
        int n;
        fscanf(file, "%d\n", &n);
        char **words = (char **)calloc(n, sizeof(char *));
        for (int i = 0; i < n; i++) {
            words[i] = (char *)calloc(15, sizeof(char));
            fgets(words[i], 15, file);
            memset(words[i] + strlen(words[i]) - 1, 0, 1);
        }
        for (int i = 1; i <= 3; i++) {
            int start = (i - 1) * (double)n / 3;
            int end = MIN(i * (double)n / 3, n);
            MPI_Send(&start, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&end, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
            for (int j = start; j < end; j++) {
                MPI_Send(words[j], 15, MPI_CHAR, i, j + 2, MPI_COMM_WORLD);
            }
        }
    } else if (rank == 1 || rank == 2 || rank == 3) {
        int start, end;
        MPI_Recv(&start, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, NULL);
        MPI_Recv(&end, 1, MPI_INT, MASTER, 1, MPI_COMM_WORLD, NULL);
        int vowels = 0, consonant = 0;
        for (int i = start; i < end; i++) {
            char *word = (char *)calloc(15, sizeof(char));
            MPI_Recv(word, 15, MPI_INT, MASTER, i + 2, MPI_COMM_WORLD, NULL);
            printf("[%d] %s\n", rank, word);
            for (int i = 0; i < strlen(word); i++) {
                if (is_vowel(word[i])) {
                    vowels++;
                } else {
                    consonant++;
                }
            }
        }
        printf("[%d] v=%d c=%d\n", rank, vowels, consonant);
        MPI_Send(&vowels, 1, MPI_INT, 4, 0, MPI_COMM_WORLD);
        MPI_Send(&consonant, 1, MPI_INT, 5, 0, MPI_COMM_WORLD);
    } else if (rank == 4) {
        int vowels = 0;
        int recv_vowels;
        for (int i = 1; i <= 3; i++) {
            MPI_Recv(&recv_vowels, 1, MPI_INT, i, 0, MPI_COMM_WORLD, NULL);
            vowels += recv_vowels;
        }
        printf("[%d] Vowels: %d\n", rank, vowels);
    } else if (rank == 5) {
        int consonants = 0;
        int recv_consonants;
        for (int i = 1; i <= 3; i++) {
            MPI_Recv(&recv_consonants, 1, MPI_INT, i, 0, MPI_COMM_WORLD, NULL);
            consonants += recv_consonants;
        }
        printf("[%d] Consonants: %d\n", rank, consonants);
    }


    MPI_Finalize();
}
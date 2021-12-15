#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MIN(a,b) (((a)<(b))?(a):(b))

pthread_barrier_t barrier;

struct add_serial_struct {
    int id;
};


int* arr;
int* positions_sum;
int* number_of_aparitions;
int N, P, X;

void *thread_function(void *arg) {
    struct add_serial_struct struture = *(struct add_serial_struct *)arg;
    int start = struture.id * (double)N / P;
    int end = MIN((struture.id + 1) * (double)N / P, N);
    for (int i = start; i < end; i++) {
        if (arr[i] == X) {
            number_of_aparitions[struture.id]++;
            positions_sum[struture.id] += i;
        }
    }

    pthread_barrier_wait(&barrier);

    if (struture.id == 0) {
        int counter_aparitions = 0;
        int sum_positions = 0;
        for (int i = 0; i < P; i++) {
            counter_aparitions += number_of_aparitions[i];
            sum_positions += positions_sum[i];
        }
        printf("%d %d\n", counter_aparitions, sum_positions);
    }

    free(arg);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    int r;

    N = atoi(argv[1]);
    P = atoi(argv[2]);
    X = atoi(argv[3]);

    arr = (int *)calloc(N, sizeof(int));
    for (int i = 0; i < N; i++) {
        arr[i] = (i + 3) % 5;
    }

    number_of_aparitions = (int *)calloc(P, sizeof(int));
    positions_sum = (int *)calloc(P, sizeof(int));

    pthread_barrier_init(&barrier, NULL, P);

    int tid[P];
    pthread_t threads[P];
  	for (int i = 0; i < P; i++) {
        struct add_serial_struct *structure = (struct add_serial_struct *)calloc(1, sizeof(struct add_serial_struct));
        structure->id = i;
        r = pthread_create(&threads[i], NULL, thread_function, structure);
        if (r) {
            printf("Eroare la crearea threadurilor");
            return 1;
        }
    }

    for (int i = 0; i < P; i++) {
        r = pthread_join(threads[i], NULL);
    }

    pthread_barrier_destroy(&barrier);
    free(arr);
    free(positions_sum);
    free(number_of_aparitions);
    return 0;
}

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define NUM_THREADS sysconf(_SC_NPROCESSORS_CONF)

struct add_serial_struct {
    int id;
};


int* arr;
int array_size;
int* sorted_arr;
int* rank;
pthread_barrier_t barrier;

void *thread_function(void *arg) {
    struct add_serial_struct struture = *(struct add_serial_struct *)arg;
    int start = struture.id * (double)array_size / NUM_THREADS;
    int end = MIN((struture.id + 1) * (double)array_size / NUM_THREADS, array_size);
    for (int i = start; i < end; i++) {
        for (int j = 0; j < i; j++) {
            if (arr[j] > arr[i]) {
                rank[j]++;
            } else {
                rank[i]++;
            }
        }
    }
    pthread_barrier_wait(&barrier);

    if (struture.id == 0) {
        for (int i = 0; i < array_size; i++) {
            sorted_arr[rank[i]] = arr[i];
        }
    }
    free(arg);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    int r;

    array_size = 10;
    pthread_barrier_init(&barrier, NULL, NUM_THREADS);

    srand(time(NULL));
    arr = (int *)calloc(array_size, sizeof(int));
    for (int i = 0; i < array_size; i++) {
        arr[i] = rand() % 30;
    }

    sorted_arr = (int *)calloc(array_size, sizeof(int));
    rank = (int *)calloc(array_size, sizeof(int));

    int tid[NUM_THREADS];
    pthread_t threads[NUM_THREADS];
  	for (int i = 0; i < NUM_THREADS; i++) {
        struct add_serial_struct *structure = (struct add_serial_struct *)calloc(1, sizeof(struct add_serial_struct));
        structure->id = i;
        r = pthread_create(&threads[i], NULL, thread_function, structure);
        if (r) {
            printf("Eroare la crearea threadurilor");
            return 1;
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        r = pthread_join(threads[i], NULL);
    }

    for (int i = 0; i < array_size; i++) {
        printf("%d ", sorted_arr[i]);
    }

    pthread_barrier_destroy(&barrier);
    return 0;
}

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define NUM_THREADS sysconf(_SC_NPROCESSORS_CONF)

/*
    schelet pentru exercitiul 5
*/

struct add_serial_struct {
    int start;
    int end;
};


int* arr;
int array_size;

void *f(void *arg) {
    struct add_serial_struct struture = *(struct add_serial_struct *)arg;
    for (int i = struture.start; i < struture.end; i++) {
        arr[i] += 100;
    }
    free(arg);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    int r;
    if (argc < 2) {
        perror("Specificati dimensiunea array-ului\n");
        exit(-1);
    }

    array_size = atoi(argv[1]);

    arr = malloc(array_size * sizeof(int));
    for (int i = 0; i < array_size; i++) {
        arr[i] = i;
    }

    // for (int i = 0; i < array_size; i++) {
    //     printf("%d", arr[i]);
    //     if (i != array_size - 1) {
    //         printf(" ");
    //     } else {
    //         printf("\n");
    //     }
    // }

    // for (int i = 0; i < array_size; i++) {
    //     arr[i] += 100;
    // }

    // TODO: aceasta operatie va fi paralelizata
    int tid[NUM_THREADS];
    pthread_t threads[NUM_THREADS];
  	for (int i = 0; i < NUM_THREADS; i++) {
        int start = i * (double)array_size / NUM_THREADS;
        int end = MIN((i + 1) * (double)array_size / NUM_THREADS, array_size);
        struct add_serial_struct *structure = (struct add_serial_struct *)calloc(1, sizeof(struct add_serial_struct));
        structure->start = start;
        structure->end = end;
        tid[i] = i;
        r = pthread_create(&threads[i], NULL, f, structure);
        if (r) {
            printf("Eroare la crearea threadurilor");
            return 1;
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        r = pthread_join(threads[i], NULL);
    }

    // for (int i = 0; i < array_size; i++) {
    //     printf("%d", arr[i]);
    //     if (i != array_size - 1) {
    //         printf(" ");
    //     } else {
    //         printf("\n");
    //     }
    // }

  	pthread_exit(NULL);
    return 0;
}

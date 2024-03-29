#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MIN(a,b) (((a)<(b))?(a):(b))

int N;
int P;
int **a;
int **b;
int **c;

struct multiply_outer_struct {
	int start;
	int end;
};

void *thread_function(void *arg)
{
	struct multiply_outer_struct struture = *(struct multiply_outer_struct *)arg;
	for (int i = struture.start; i < struture.end; i++) {
		for (int j = 0; j < N; j++) {
			for (int k = 0; k < N; k++) {
				c[i][j] += a[i][k] * b[k][j];
			}
		}
	}

	free(arg);
	pthread_exit(NULL);
}

void get_args(int argc, char **argv)
{
	if(argc < 3) {
		printf("Numar insuficient de parametri: ./program N P\n");
		exit(1);
	}

	N = atoi(argv[1]);
	P = atoi(argv[2]);
}

void init()
{
	a = malloc(sizeof(int *) * N);
	b = malloc(sizeof(int *) * N);
	c = malloc(sizeof(int *) * N);

	if (a == NULL || b == NULL || c == NULL) {
		printf("Eroare la malloc!");
		exit(1);
	}

	int i, j;
	for (i = 0; i < N; i++) {
		a[i] = malloc(sizeof(int) * N);
		b[i] = malloc(sizeof(int) * N);
		c[i] = malloc(sizeof(int) * N);

		if(a[i] == NULL || b[i] == NULL || c[i] == NULL) {
			printf("Eroare la malloc!");
			exit(1);
		}
	}

	for (i = 0; i < N; i++) {
		for(j = 0; j < N; j++) {
			c[i][j] = 0;

			if(i <= j) {
				a[i][j] = 1;
				b[i][j] = 1;
			} else {
				a[i][j] = 0;
				b[i][j] = 0;
			}
		}
	}
}

void print(int **mat)
{
	int i, j;

	for (i = 0; i < N; i++) {
		for(j = 0; j < N; j++) {
			printf("%i\t", mat[i][j]);
		}
		printf("\n");
	}
}

int main(int argc, char *argv[])
{
	int i;

	get_args(argc, argv);
	init();

	pthread_t tid[P];
	int thread_id[P];

	for (i = 0; i < P; i++) {
		thread_id[i] = i;
		int start = i * (double)N / P;
        int end = MIN((i + 1) * (double)N / P, N);
		struct multiply_outer_struct *structure = (struct multiply_outer_struct *)calloc(1, sizeof(struct multiply_outer_struct));
        structure->start = start;
        structure->end = end;
		pthread_create(&tid[i], NULL, thread_function, structure);
	}

	for (i = 0; i < P; i++) {
		pthread_join(tid[i], NULL);
	}

	print(c);

	return 0;
}

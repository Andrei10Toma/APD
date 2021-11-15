#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "genetic_algorithm.h"
 
int read_input(sack_object **objects, int *num_threads, int *object_count, int *sack_capacity, int *generations_count, int argc, char *argv[]) 
{
	FILE *fp;

	if (argc < 4) {
		fprintf(stderr, "Usage:\n\t./tema1 in_file generations_count num_threads\n");
		return 0;
	}

	fp = fopen(argv[1], "r");
	if (fp == NULL) {
		return 0;
	}

	if (fscanf(fp, "%d %d", object_count, sack_capacity) < 2) {
		fclose(fp);
		return 0;
	}

	if (*object_count % 10) {
		fclose(fp);
		return 0;
	}

	sack_object *tmp_objects = (sack_object *) calloc(*object_count, sizeof(sack_object));

	for (int i = 0; i < *object_count; ++i) {
		if (fscanf(fp, "%d %d", &tmp_objects[i].profit, &tmp_objects[i].weight) < 2) {
			free(objects);
			fclose(fp);
			return 0;
		}
	}

	fclose(fp);

	*generations_count = (int) strtol(argv[2], NULL, 10);
	
	if (*generations_count == 0) {
		free(tmp_objects);

		return 0;
	}

	*objects = tmp_objects;
	*num_threads = atoi(argv[3]);

	return 1;
}

void print_objects(const sack_object *objects, int object_count)
{
	for (int i = 0; i < object_count; ++i) {
		printf("%d %d\n", objects[i].weight, objects[i].profit);
	}
}

void print_generation(const individual *generation, int limit)
{
	for (int i = 0; i < limit; ++i) {
		for (int j = 0; j < generation[i].chromosome_length; ++j) {
			printf("%d ", generation[i].chromosomes[j]);
		}

		printf("\n%d - %d\n", i, generation[i].fitness);
	}
}

void print_best_fitness(const individual *generation)
{
	printf("%d\n", generation[0].fitness);
}

void compute_fitness_function(const sack_object *objects, individual *generation, int object_count, int sack_capacity, int start)
{
	int weight;
	int profit;

	for (int i = start; i < object_count; ++i) {
		weight = 0;
		profit = 0;

		for (int j = 0; j < generation[i].chromosome_length; ++j) {
			if (generation[i].chromosomes[j]) {
				weight += objects[j].weight;
				profit += objects[j].profit;
			}
		}

		generation[i].fitness = (weight <= sack_capacity) ? profit : 0;
	}
}

int cmpfunc(const void *a, const void *b)
{
	int i;
	individual *first = (individual *) a;
	individual *second = (individual *) b;

	int res = second->fitness - first->fitness; // decreasing by fitness
	if (res == 0) {
		int first_count = 0, second_count = 0;

		for (i = 0; i < first->chromosome_length && i < second->chromosome_length; ++i) {
			first_count += first->chromosomes[i];
			second_count += second->chromosomes[i];
		}

		res = first_count - second_count; // increasing by number of objects in the sack
		if (res == 0) {
			return second->index - first->index;
		}
	}

	return res;
}

void mutate_bit_string_1(const individual *ind, int generation_index)
{
	int i, mutation_size;
	int step = 1 + generation_index % (ind->chromosome_length - 2);

	if (ind->index % 2 == 0) {
		// for even-indexed individuals, mutate the first 40% chromosomes by a given step
		mutation_size = ind->chromosome_length * 4 / 10;
		for (i = 0; i < mutation_size; i += step) {
			ind->chromosomes[i] = 1 - ind->chromosomes[i];
		}
	} else {
		// for even-indexed individuals, mutate the last 80% chromosomes by a given step
		mutation_size = ind->chromosome_length * 8 / 10;
		for (i = ind->chromosome_length - mutation_size; i < ind->chromosome_length; i += step) {
			ind->chromosomes[i] = 1 - ind->chromosomes[i];
		}
	}
}

void mutate_bit_string_2(const individual *ind, int generation_index)
{
	int step = 1 + generation_index % (ind->chromosome_length - 2);

	// mutate all chromosomes by a given step
	for (int i = 0; i < ind->chromosome_length; i += step) {
		ind->chromosomes[i] = 1 - ind->chromosomes[i];
	}
}

void crossover(individual *parent1, individual *child1, int generation_index)
{
	individual *parent2 = parent1 + 1;
	individual *child2 = child1 + 1;
	int count = 1 + generation_index % parent1->chromosome_length;

	memcpy(child1->chromosomes, parent1->chromosomes, count * sizeof(int));
	memcpy(child1->chromosomes + count, parent2->chromosomes + count, (parent1->chromosome_length - count) * sizeof(int));

	memcpy(child2->chromosomes, parent2->chromosomes, count * sizeof(int));
	memcpy(child2->chromosomes + count, parent1->chromosomes + count, (parent1->chromosome_length - count) * sizeof(int));
}

void copy_individual(const individual *from, const individual *to)
{
	memcpy(to->chromosomes, from->chromosomes, from->chromosome_length * sizeof(int));
}

void free_generation(individual *generation)
{
	int i;

	for (i = 0; i < generation->chromosome_length; ++i) {
		free(generation[i].chromosomes);
		generation[i].chromosomes = NULL;
		generation[i].fitness = 0;
	}
}

void my_copy_individual(individual *from, individual *to) {
	to->chromosome_length = from->chromosome_length;
	to->fitness = from->fitness;
	to->index = from->index;
	to->chromosomes = from->chromosomes;
}

individual* mergeArrays(individual *arr1, individual *arr2, int n1, int n2) {
	individual *result = (individual *)calloc(n1 + n2, sizeof(individual));
    int i = 0, j = 0, k = 0;
	
    while (i < n1 && j < n2) {
        if (cmpfunc(arr1 + i, arr2 + j) < 0)
            memcpy(result + k, arr1 + (i++), sizeof(individual));
        else
            memcpy(result + k, arr2 + (j++), sizeof(individual));
		k++;
    }
 
    while (i < n1) {
        memcpy(result + k, arr1 + (i++), sizeof(individual));
		k++;
	}
    while (j < n2) {
        memcpy(result + k, arr2 + (j++), sizeof(individual));
		k++;
	}
	return result;
}

// create the structure that will be given as argument for the function of the threads
thread_params* create_thread_params(int thread_id, int object_count, individual *current_generation,
	individual *next_generation, int num_threads, pthread_barrier_t *barrier, int generations_count, 
	const sack_object *objects, int sack_capacity) {
	thread_params* _thread_params = (thread_params *)calloc(1, sizeof(thread_params));
	_thread_params->thread_id = thread_id;
	_thread_params->object_count = object_count;
	_thread_params->num_threads = num_threads;
	_thread_params->generations_count = generations_count;
	_thread_params->next_generation = next_generation;
	_thread_params->current_generation = current_generation;
	_thread_params->barrier = barrier;
	_thread_params->objects = objects;
	_thread_params->sack_capacity = sack_capacity;
	return _thread_params;
}

void *thread_function(void *arg) {
	thread_params *_thread_params = (thread_params *) arg;
	int i, k, cursor, count;
	int thread_id = _thread_params->thread_id;
	int object_count = _thread_params->object_count;
	int num_threads = _thread_params->num_threads;
	individual *current_generation = _thread_params->current_generation;
	individual *next_generation = _thread_params->next_generation;
	individual *tmp = NULL;
	int generations_count = _thread_params->generations_count;
	int start = thread_id * (double)object_count / num_threads;
    int end = MIN((thread_id + 1) * (double)object_count / num_threads, object_count);
	pthread_barrier_t *barrier = _thread_params->barrier;
	const sack_object *objects = _thread_params->objects;
	int sack_capacity = _thread_params->sack_capacity;

	for (i = start; i < end; ++i) {
		current_generation[i].fitness = 0;
		current_generation[i].chromosomes = (int*) calloc(object_count, sizeof(int));
		current_generation[i].chromosomes[i] = 1;
		current_generation[i].index = i;
		current_generation[i].chromosome_length = object_count;

		next_generation[i].fitness = 0;
		next_generation[i].chromosomes = (int*) calloc(object_count, sizeof(int));
		next_generation[i].index = i;
		next_generation[i].chromosome_length = object_count;
	}
	pthread_barrier_wait(barrier);

	for (k = 0; k < generations_count; k++) {
		cursor = 0;

		start = thread_id * (double) object_count / num_threads;
		end = MIN((thread_id + 1) * (double) object_count / num_threads, object_count);
		compute_fitness_function(objects, current_generation, end, sack_capacity, start);

		// wait for all the threads to compute the fitness of the objects
		pthread_barrier_wait(barrier);

		start = thread_id * (double) object_count / num_threads;
		end = MIN((thread_id + 1) * (double) object_count / num_threads, object_count);
		// sort the objests in parallel (no_threads sorted arrays)
		qsort(current_generation + start, end - start, sizeof(individual), cmpfunc);
		// wait for all threads to finish the sorting
		// single thread merge sort all the no_threads arrays
		pthread_barrier_wait(barrier);
		if (thread_id == 0) {
			int start1 = 0;
			int end1 = MIN((double) object_count / num_threads, object_count);
			for (i = 0; i < num_threads - 1; i++) {
				int start2 = (i + 1) * (double) object_count / num_threads;
				int end2 = MIN((i + 2) * (double) object_count / num_threads, object_count);
				individual *destination_sorted_array = mergeArrays(current_generation + start1, current_generation + start2, end1 - start1, end2 - start2);
				memcpy(current_generation, destination_sorted_array, sizeof(individual) * (end2 - start1));
				end1 = end2;
				free(destination_sorted_array);
			}
		}
		// wait for the merge sort to be finished
		pthread_barrier_wait(barrier);
		count = object_count * 3 / 10;
		start = thread_id * (double) count / num_threads;
		end = MIN((thread_id + 1) * (double)count / num_threads, count);
		// parallelize elite selection
		for (i = start; i < end; ++i) {
			copy_individual(current_generation + i, next_generation + i);
		}

		cursor = count;

		// wait for all the threads to finish the job
		pthread_barrier_wait(barrier);

		count = object_count * 2 / 10;
		start = thread_id * (double) count / num_threads;
		end = MIN((thread_id + 1) * (double)count / num_threads, count);
		// paralelize the first mutation
		for (i = start; i < end; ++i) {
			copy_individual(current_generation + i, next_generation + cursor + i);
			mutate_bit_string_1(next_generation + cursor + i, k);
		}

		cursor += count;
		pthread_barrier_wait(barrier);

		count = object_count * 2 / 10;
		start = thread_id * (double) count / num_threads;
		end = MIN((thread_id + 1) * (double)count / num_threads, count);
		// paralelize the second mutation
		for (i = start; i < end; ++i) {
			copy_individual(current_generation + i + count, next_generation + cursor + i);
			mutate_bit_string_2(next_generation + cursor + i, k);
		}
		cursor += count;

		pthread_barrier_wait(barrier);

		count = object_count * 3 / 10;
		if (count % 2 == 1) {
			copy_individual(current_generation + object_count - 1, next_generation + cursor + count - 1);
			count--;
		}

		pthread_barrier_wait(barrier);
		// paralelize the crossover

		start = thread_id * (double) count / num_threads;
		end = MIN((thread_id + 1) * (double)count / num_threads, count);
		for (i = start; i < end; i += 2) {
			if (i < count - 1)
				crossover(current_generation + i, next_generation + cursor + i, k);
		}

		pthread_barrier_wait(barrier);

		tmp = current_generation;
		current_generation = next_generation;
		next_generation = tmp;

		start = thread_id * (double) object_count / num_threads;
		end = MIN((thread_id + 1) * (double)object_count / num_threads, object_count);
		for (i = start; i < end; ++i) {
			current_generation[i].index = i;
		}

		if (k % 5 == 0 && thread_id == 0) {
			print_best_fitness(current_generation);
		}
	}

	pthread_exit(NULL);
}

void run_genetic_algorithm(const sack_object *objects, int object_count, int generations_count, int sack_capacity, int num_threads)
{
	pthread_barrier_t *barrier = (pthread_barrier_t *)calloc(1, sizeof(pthread_barrier_t));
	pthread_t *threads = (pthread_t *)calloc(num_threads, sizeof(pthread_t));
	if (threads == NULL) {
		perror("Eroare la alocarea threadurilor");
		return;
	}
	int *thread_ids = (int *)calloc(num_threads, sizeof(int));
	if (thread_ids == NULL) {
		perror("Eroare la alocarea thread id-urilor");
		free(threads);
		return;
	}
	int return_value, i;
	return_value = pthread_barrier_init(barrier, NULL, num_threads);
	if (return_value < 0) {
		perror("Eroare la initializarea barierei");
		free(threads);
		free(thread_ids);
		return;
	}

	individual *current_generation = (individual*) calloc(object_count, sizeof(individual));
	individual *next_generation = (individual*) calloc(object_count, sizeof(individual));

	for (i = 0; i < num_threads; i++) {
		// start the threads
		thread_ids[i] = i;
		// create the thread parameters
		thread_params *thread_params = create_thread_params(thread_ids[i], object_count, current_generation, 
			next_generation, num_threads, barrier, generations_count, objects, sack_capacity);
		return_value = pthread_create(&threads[i], NULL, thread_function, thread_params);
		if (return_value < 0) {
			perror("Eroare la crearea threadurilor");
			return;
		}
	}

	// join the threads
	for (i = 0; i < num_threads; i++) {
		return_value = pthread_join(threads[i], NULL);
		if (return_value < 0) {
			perror("Eroare la join-ul de threaduri");
			return;
		}
	}

	// compute the best fitness from the current_generation
	compute_fitness_function(objects, current_generation, object_count, sack_capacity, 0);
	qsort(current_generation, object_count, sizeof(individual), cmpfunc);
	print_best_fitness(current_generation);

	// free resources for old generation
	free_generation(current_generation);
	free_generation(next_generation);

	// free resources
	free(current_generation);
	free(next_generation);
	pthread_barrier_destroy(barrier);
	free(barrier);
	free(threads);
	free(thread_ids);
}
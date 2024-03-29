#include <stdlib.h>
#include "genetic_algorithm.h"

int main(int argc, char *argv[]) {
	// array with all the objects that can be placed in the sack
	sack_object *objects = NULL;

	// number of objects
	int object_count = 0;

	// maximum weight that can be carried in the sack
	int sack_capacity = 0;

	// number of generations
	int generations_count = 0;

	int num_threads = 0;

	if (!read_input(&objects, &num_threads, &object_count, &sack_capacity, &generations_count, argc, argv)) {
		return 0;
	}

	run_genetic_algorithm(objects, object_count, generations_count, sack_capacity, num_threads);

	free(objects);

	return 0;
}

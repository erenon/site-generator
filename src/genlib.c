/*
 * @author erenon
 */

#include <stdlib.h>
#include <stdio.h>

/**
 * Allocates a requested amount of memory
 * Checks whether malloc was succesful or not.
 *
 * @warning Aborts if out of memory
 *
 * @param size requested memory size
 */
void *smalloc(size_t size) {
	void *p = NULL;
	p = malloc(size);

	if (p == NULL) {
	    fprintf(stderr, "Out of memory! Aborting...\n");
	    exit(EXIT_FAILURE);
	}

	return p;
}

/**
 * Frees the given pointer
 * and sets it's value to null.
 *
 * Prints error if null pointer given
 */
void sfree(void *pointer) {
	if (pointer != NULL) {
		free(pointer);
		pointer = NULL;
	} else {
		fprintf(stderr, "Freeing NULL pointer! Do nothing.\n");
	}
}

/*
 * genlib.c
 *
 *  Created on: 2010.10.29.
 *      Author: erenon
 */

#include <stdlib.h>
#include <stdio.h>

void *smalloc(size_t size) {
	void *p = NULL;
	p = malloc(size);

	if (p == NULL) {
	    fprintf(stderr, "Out of memory! Aborting...\n");
	    exit(EXIT_FAILURE);
	}

	return p;
}

void *sfree(void *pointer) {
	if (pointer != NULL) {
		free(pointer);
		pointer = NULL;
	} else {
		fprintf(stderr, "Freeing NULL pointer! Do nothing.\n");
	}
}

void *semalloc(size_t count, void *p) {
	return (typeof(*p) *)smalloc(count*sizeof(typeof(*p)));
}
